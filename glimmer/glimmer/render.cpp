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
{

}

MyRender::~MyRender()
{

}

bool MyRender::PreInit(Core::ISystemEnumerator& systemEnumerator)
{
	auto inputSystem = (Input::InputSystem*)systemEnumerator.GetSystem("Input");
	auto renderSystem = (SDE::RenderSystem*)systemEnumerator.GetSystem("Render");
	auto jobSystem = (SDE::JobSystem*)systemEnumerator.GetSystem("Jobs");
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
	std::vector<uint8_t> rawRenderOutput;
	rawRenderOutput.resize(c_outputSizeX*c_outputSizeY*4);

	// do the rendering
	TraceParamaters params = { rawRenderOutput, c_outputSizeX, c_outputSizeY };
	TraceMeSomethingNice(params);
	
	// create a new texture (slow!)
	
	// this particularly sucks
	std::vector<Render::TextureSource::MipDesc> mips;
	mips.push_back({ c_outputSizeX, c_outputSizeY, 0, c_outputSizeX * c_outputSizeY * 4 * sizeof(char) });
	Render::TextureSource textureDescriptor(c_outputSizeX, c_outputSizeY, Render::TextureSource::Format::RGBA8, mips, rawRenderOutput);
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

	m_debugGui->Image(*m_outputTexture, { c_outputSizeX, c_outputSizeY });

	return true;
}

bool MyRender::Tick()
{
	return RenderFrame();
}

void MyRender::Shutdown()
{
	m_outputTexture = nullptr;
}