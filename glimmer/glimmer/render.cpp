#include "render.h"

#include "core/system_enumerator.h"
#include "debug_gui/debug_gui_system.h"
#include "sde/render_system.h"
#include "sde/debug_render.h"
#include "sde/job_system.h"

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

bool MyRender::Tick()
{
	return true;
}

void MyRender::Shutdown()
{
}