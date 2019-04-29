#include "cpu_raytracer.h"
#include "kernel/assert.h"
#include "kernel/thread.h"
#include "core/timer.h"
#include "render/texture.h"
#include "render/texture_source.h"
#include "render/camera.h"
#include "sde/job_system.h"

#include "core/system_enumerator.h"
#include "sde/script_system.h"
#include "debug_gui/debug_gui_system.h"

const glm::ivec2 c_outputSize = { 512, 512 };

void CpuRaytracerSystem::CreateScene()
{
	TraceBoi::RegisterScriptTypes(m_scriptSystem->Globals(), m_scene);

	// Load the scene
	std::string errorText;
	if (!m_scriptSystem->RunScriptFromFile("scene.lua", errorText))
	{
		SDE_LOG("Lua error in scene.lua - %s", errorText.data());
	}

	// Setup the camera
	m_camera.SetFOVAndAspectRatio(51.52f, (float)c_outputSize.x / (float)c_outputSize.y);
	m_camera.LookAt({ 0.0f, 50.0f, -200.0f }, { 0.0f, 50.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
}

bool CpuRaytracerSystem::PreInit(Core::ISystemEnumerator& systemEnumerator)
{
	m_debugGui = (DebugGui::DebugGuiSystem*)systemEnumerator.GetSystem("DebugGui");
	m_scriptSystem = (SDE::ScriptSystem*)systemEnumerator.GetSystem("Script");

	// Set up cpu ray tracer
	CpuRaytracer::Parameters params;
	params.m_jobSystem = (SDE::JobSystem*)systemEnumerator.GetSystem("Jobs");
	params.m_maxRecursion = 6;
	params.m_image.m_dimensions = c_outputSize;
	m_cpuTracer = std::make_unique<CpuRaytracer>(params);

	return true;
}

bool CpuRaytracerSystem::PostInit()
{
	CreateScene();
	return true;
}

void CpuRaytracerSystem::UpdateSceneControls()
{
	static bool s_controlsOpen = true;
	m_debugGui->BeginWindow(s_controlsOpen, "Controls", { 512,512 });

	auto frand = [](float min, float max)
	{
		return min + (rand() / (float)RAND_MAX) * fabs(max - min);
	};

	for (int s = 0; s < m_scene.spheres.size(); ++s)
	{
		char label[256] = { '\0' };
		sprintf_s(label, "Sphere %d", s);
		m_debugGui->DragVector(label, m_scene.spheres[s].m_sphere.m_posAndRadius, 0.25f);
		sprintf_s(label, "Sphere Material %d", s);
		m_debugGui->DragFloat(label, m_scene.spheres[s].m_material.m_refractiveIndex, 0.01f, -1.0f, 10.0f);
	}
	if (m_debugGui->Button("Add Sphere"))
	{
		m_scene.spheres.push_back({
			glm::vec4(frand(-50.0f,50.0f), frand(0.0f,50.0f), frand(0.0f,50.0f), frand(2.0, 20.0f)), {1.01f, Diffuse}
			});
	}
	m_debugGui->Separator();
	for (int l = 0; l < m_scene.lights.size(); ++l)
	{
		char label[256] = { '\0' };
		sprintf_s(label, "Light %d Position", l);
		m_debugGui->DragVector(label, m_scene.lights[l].m_position, 0.25f);
		sprintf_s(label, "Light %d Colour", l);
		m_debugGui->DragVector(label, m_scene.lights[l].m_diffuse, 0.05f, 0.0f, 1.0f);
	}
	m_debugGui->Separator();
	glm::vec4 c = glm::vec4(m_scene.skyColour, 1.0f);
	m_debugGui->ColourEdit("Sky Colour", c);
	m_scene.skyColour = { c.x, c.y, c.z };
	if (m_debugGui->Button("Add Light"))
	{
		m_scene.lights.push_back({
			{frand(-250.0f,250.0f),frand(50.0f,500.0f), frand(-250.0f,250.0f)},
			{frand(0.0f,0.5f), frand(0.0f,0.5f), frand(0.0f,0.5f)}
			});
	}
	m_debugGui->Separator();
	glm::vec3 cameraPos = m_camera.Position();
	m_debugGui->DragVector("Camera Position", cameraPos, 0.1f);
	m_camera.SetPosition(cameraPos);
	glm::vec3 cameraT = m_camera.Target();
	m_debugGui->DragVector("Camera Target", cameraT, 0.1f);
	m_camera.SetTarget(cameraT);

	m_debugGui->EndWindow();
}

void CpuRaytracerSystem::UpdateControls()
{
	static bool s_controlsOpen = true;
	m_debugGui->BeginWindow(s_controlsOpen, "Controls", { 512,512 });

	char text[256] = { '\0' };
	sprintf_s(text, "Raytrace time: %fs", m_cpuTracer->GetLastDrawTime());
	m_debugGui->Text(text);

	const char* statusAsText[] = {
		"Ready",
		"In Progress",
		"Complete",
		"Paused",
		"_unknown"
	};
	sprintf_s(text, "Status: %s\n", statusAsText[m_cpuTracer->GetStatus()]);
	m_debugGui->Text(text);

	m_debugGui->Checkbox("Paused", &m_isPaused);
	m_debugGui->EndWindow();
}

bool CpuRaytracerSystem::Tick()
{
	UpdateControls();
	UpdateSceneControls();

	if (!m_isPaused)
	{
		m_cpuTracer->TryDrawScene(m_scene, m_camera);
	}
	m_cpuTracer->Tick();

	// Use imgui as a free blitter!
	if (m_cpuTracer->GetTexture() != nullptr)
	{
		bool stayOpen = true;
		m_debugGui->BeginWindow(stayOpen, "Cpu Output", glm::vec2(c_outputSize + glm::ivec2(16,32)));
		m_debugGui->Image(*m_cpuTracer->GetTexture(), glm::vec2(c_outputSize));
		m_debugGui->EndWindow();
	}

	return true;
}

void CpuRaytracerSystem::Shutdown()
{
	m_cpuTracer = nullptr;
}

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

bool CpuRaytracer::TryDrawScene(Scene& scene, Render::Camera& camera)
{
	int traceReady = Status::Ready;
	if (m_traceStatus.compare_exchange_strong(traceReady, Status::InProgress))
	{
		SubmitRenderJobs(scene, camera);
		return true;
	}
	return false;
}

void CpuRaytracer::SubmitRenderJobs(Scene& scene, Render::Camera& camera)
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
		TraceParamaters params = { m_rawOutput, scene, camera, imageDimensions, origin, dimensions, m_parameters.m_maxRecursion };
		m_parameters.m_jobSystem->PushJob([=]()
		{
			TraceBoi::TraceMeSomethingNice(params);
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