#include "cpu_raytracer.h"
#include "kernel/assert.h"
#include "kernel/thread.h"
#include "core/timer.h"
#include "render/texture.h"
#include "render/texture_source.h"
#include "sde/job_system.h"

CpuRaytracer::CpuRaytracer(const Parameters& params)
	: m_parameters(params)
	, m_jobsInProgress(0)
	, m_traceStatus(Status::Ready)
	, m_traceStartTime(0)
	, m_lastTraceTime(0)
{
	m_rawOutput.resize(params.m_image.m_dimensions.x * params.m_image.m_dimensions.y);
}

CpuRaytracer::~CpuRaytracer()
{
	while (m_traceStatus == Status::InProgress)
	{
		Kernel::Thread::Sleep(10);	//nasty
	}
}

Render::Texture* CpuRaytracer::GetTexture()
{
	return m_texture.get();
}

bool CpuRaytracer::TryDrawScene(Scene& scene)
{
	int traceReady = Status::Ready;
	if (m_traceStatus.compare_exchange_strong(traceReady, Status::InProgress))
	{
		SubmitRenderJobs(scene);
		return true;
	}
	return false;
}

void CpuRaytracer::SubmitRenderJobs(Scene& scene)
{
	SDE_ASSERT(m_parameters.m_image.m_dimensions.y % m_parameters.m_jobCount == 0);
	SDE_ASSERT(m_traceStatus == Status::InProgress);

	Core::Timer jobTimer;
	m_traceStartTime = jobTimer.GetSeconds();
	m_jobsInProgress = m_parameters.m_jobCount;
	auto imageDimensions = glm::ivec2(m_parameters.m_image.m_dimensions.x, m_parameters.m_image.m_dimensions.y);

	int rowsPerJob = m_parameters.m_image.m_dimensions.y / m_parameters.m_jobCount;
	for (int j = 0; j < m_parameters.m_jobCount; ++j)
	{
		// split the image into horizontal strips 
		glm::ivec2 origin(0, j * rowsPerJob);
		glm::ivec2 dimensions(m_parameters.m_image.m_dimensions.x, rowsPerJob);
		TraceParamaters params = { m_rawOutput, scene, imageDimensions, origin, dimensions, 8 };
		m_parameters.m_jobSystem->PushJob([=]()
		{
			TraceMeSomethingNice(params);
			if (--m_jobsInProgress <= 0)
			{
				m_lastTraceTime = jobTimer.GetSeconds() - m_traceStartTime;
				m_traceStatus = Status::Complete;
			}
		});
	}
}

void CpuRaytracer::UpdateTextureFromResult()
{
	glm::uvec2 imageDims = { m_parameters.m_image.m_dimensions.x, m_parameters.m_image.m_dimensions.y };

	std::vector<Render::TextureSource::MipDesc> mips;
	mips.push_back({ imageDims.x, imageDims.y, 0, sizeof(uint32_t) * imageDims.x * imageDims.y });
	Render::TextureSource textureDescriptor(imageDims.x, imageDims.y, Render::TextureSource::Format::RGBA8, mips, m_rawOutput);
	std::vector<Render::TextureSource> textureSources;
	textureSources.push_back(textureDescriptor);

	CreateOrUpdateTexture(textureSources);
}

void CpuRaytracer::CreateOrUpdateTexture(const std::vector<Render::TextureSource>& ts)
{
	if (m_texture == nullptr)
	{
		m_texture = std::make_unique<Render::Texture>();
		if (!m_texture->Create(ts))
		{
			m_texture = nullptr;
		}
	}
	else
	{
		m_texture->Update(ts);
	}
}

void CpuRaytracer::Tick()
{
	// If we can switch from complete -> ready, then the last job finished
	int traceComplete = Status::Complete;
	if (m_traceStatus.compare_exchange_strong(traceComplete, Status::Ready))
	{
		UpdateTextureFromResult();
	}
}