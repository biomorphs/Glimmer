#include "render.h"
#include "core/system_enumerator.h"
#include "render/texture.h"
#include "render/texture_source.h"
#include "debug_gui/debug_gui_system.h"
#include "sde/render_system.h"
#include "sde/debug_render.h"
#include "sde/job_system.h"
#include "traceboi.h"
#include <vector>

const uint32_t c_outputSizeX = 512;
const uint32_t c_outputSizeY = 512;

MyRender::MyRender(int windowResX, int windowResY)
	: m_windowResolution(windowResX, windowResY)
	, m_forwardPassId(-1)
	, m_debugGui(nullptr)
	, m_jobSystem(nullptr)
	, m_traceStatus(Trace::Ready)

{
	m_traceResult.resize(c_outputSizeX * c_outputSizeY * 4);
}

MyRender::~MyRender()
{

}

bool MyRender::PreInit(Core::ISystemEnumerator& systemEnumerator)
{
	auto inputSystem = (Input::InputSystem*)systemEnumerator.GetSystem("Input");
	auto renderSystem = (SDE::RenderSystem*)systemEnumerator.GetSystem("Render");
	m_jobSystem = (SDE::JobSystem*)systemEnumerator.GetSystem("Jobs");
	m_debugGui = (DebugGui::DebugGuiSystem*)systemEnumerator.GetSystem("DebugGui");

	// Pass init params to renderer
	SDE::RenderSystem::InitialisationParams renderParams(m_windowResolution.x, m_windowResolution.y, false, "AppSkeleton");
	renderSystem->SetInitialiseParams(renderParams);

	// Set up render passes
	m_forwardPassId = renderSystem->CreatePass("Forward");

	uint32_t guiPassId = renderSystem->CreatePass("DebugGui");
	renderSystem->GetPass(guiPassId).GetRenderState().m_blendingEnabled = true;
	renderSystem->GetPass(guiPassId).GetRenderState().m_depthTestEnabled = false;
	renderSystem->GetPass(guiPassId).GetRenderState().m_backfaceCullEnabled = false;

	// Set up debug gui data
	DebugGui::DebugGuiSystem::InitialisationParams guiParams(renderSystem, inputSystem, guiPassId);
	m_debugGui->SetInitialiseParams(guiParams);

	renderSystem->SetClearColour(glm::vec4(0.14f, 0.23f, 0.45f, 1.0f));

	return true;
}

bool MyRender::PostInit()
{
	return true;
}

bool MyRender::RenderFrame()
{
	if (m_outputTexture)
	{
		m_debugGui->Image(*m_outputTexture, { c_outputSizeX, c_outputSizeY });
	}

	char text[256] = { '\0' };
	char* statusTxt = nullptr;
	switch (m_traceStatus)
	{
	case Ready:
		statusTxt = (char*)"Ready";
		break;
	case InProgress:
		statusTxt = (char*)"In Progress";
		break;
	case Complete:
		statusTxt = (char*)"Complete";
		break;
	case Paused:
		statusTxt = (char*)"Paused";
		break;
	}
	sprintf_s(text, "Status: %s\n", statusTxt);
	m_debugGui->Text(text);


	static bool isPaused = false;
	m_debugGui->Checkbox("Paused", &isPaused);
	if(isPaused)
	{
		int traceReady = Trace::Ready;
		m_traceStatus.compare_exchange_strong(traceReady, Trace::Paused);
	}
	else
	{
		int tracePaused = Trace::Paused;
		m_traceStatus.compare_exchange_strong(tracePaused, Trace::Ready);
	}

	return true;
}

bool MyRender::Tick()
{
	// If we can switch from ready->inprogress, its go time
	int traceReady = Trace::Ready;
	if (m_traceStatus.compare_exchange_strong(traceReady, Trace::InProgress))
	{
		m_jobSystem->PushJob([this]()
		{
			TraceParamaters params = { m_traceResult, c_outputSizeX, c_outputSizeY };
			TraceMeSomethingNice(params);
			m_traceStatus = Trace::Complete;
		});
	}

	int traceComplete = Trace::Complete;
	if (m_traceStatus.compare_exchange_strong(traceComplete, Trace::Ready))
	{
		// update the texture
		std::vector<Render::TextureSource::MipDesc> mips;
		mips.push_back({ c_outputSizeX, c_outputSizeY, 0, c_outputSizeX * c_outputSizeY * 4 * sizeof(char) });
		Render::TextureSource textureDescriptor(c_outputSizeX, c_outputSizeY, Render::TextureSource::Format::RGBA8, mips, m_traceResult);
		std::vector<Render::TextureSource> textureSources;
		textureSources.push_back(textureDescriptor);
		if (m_outputTexture == nullptr)
		{
			m_outputTexture = std::make_unique<Render::Texture>();
			if (!m_outputTexture->Create(textureSources))
			{
				m_outputTexture = nullptr;
				return false;
			}
		}
		else
		{
			m_outputTexture->Update(textureSources);
		}
	}

	return RenderFrame();
}

void MyRender::Shutdown()
{
	m_outputTexture = nullptr;
}