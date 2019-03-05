#include "render.h"
#include "core/system_enumerator.h"
#include "debug_gui/debug_gui_system.h"
#include "sde/render_system.h"
#include "sde/debug_render.h"
#include "traceboi.h"
#include <vector>

const uint32_t c_outputSizeX = 768;
const uint32_t c_outputSizeY = 768;

void MyRender::CreateScene()
{
	std::vector<Sphere> spheres = {
		{ glm::vec4(0.0f,-100.25f,-150.0f,100.5f), {1.05f, Diffuse} },
		{ glm::vec4(3.25f,-0.75f,-16.25f,2.0f), {0.2f, Diffuse} },
		{ glm::vec4(-3.5f,-1.75f,-15.5f,1.25f), {0.18f, Diffuse} },
		{ glm::vec4(-1.0f,4.25f,-20.25f,4.0f), {0.45f, Diffuse} }
	};

	std::vector<Light> lights = {
		{ {-100.0f,35.0f,50.0f}, {0.75f,0.0f,0.0f,1.0f} },
		{ {177.25,131.0f,250.0f}, {0.25f,0.15f,0.65f,1.0f} },
	};

	glm::vec4 skyColour = glm::vec4(0.4f, 0.42f, 0.5f, 1.0f);

	m_scene = { spheres, lights, skyColour };
}

MyRender::MyRender(int windowResX, int windowResY)
	: m_windowResolution(windowResX, windowResY)
	, m_debugGui(nullptr)
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
	uint32_t guiPassId = renderSystem->CreatePass("DebugGui");
	renderSystem->GetPass(guiPassId).GetRenderState().m_blendingEnabled = true;
	renderSystem->GetPass(guiPassId).GetRenderState().m_depthTestEnabled = false;
	renderSystem->GetPass(guiPassId).GetRenderState().m_backfaceCullEnabled = false;
	renderSystem->SetClearColour(glm::vec4(0.0f,0.0f,0.0f,1.0f));

	// Set up debug gui
	DebugGui::DebugGuiSystem::InitialisationParams guiParams(renderSystem, inputSystem, guiPassId);
	m_debugGui->SetInitialiseParams(guiParams);

	// Set up cpu ray tracer
	CpuRaytracer::Parameters params;
	params.m_jobCount = 8;
	params.m_jobSystem = jobSystem;
	params.m_image.m_dimensions = { c_outputSizeX, c_outputSizeY };
	m_cpuTracer = std::make_unique<CpuRaytracer>(params);

	return true;
}

bool MyRender::PostInit()
{
	CreateScene();
	return true;
}

float frand(float min, float max)
{
	return min + (rand() / (float)RAND_MAX) * fabs(max - min);
}

void MyRender::UpdateSceneControls()
{
	static bool s_controlsOpen = true;
	m_debugGui->BeginWindow(s_controlsOpen, "Controls", { 512,512 });

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
			glm::vec4(frand(-10.0f,10.0f), frand(0.0f,10.0f), frand(-50.0f,-10.0f), frand(1.0, 8.0f)), {1.05f, Diffuse}
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
	m_debugGui->ColourEdit("Sky Colour", m_scene.skyColour);
	if (m_debugGui->Button("Add Light"))
	{
		m_scene.lights.push_back({
			{frand(-50.0f,50.0f),frand(20.0f,500.0f), frand(-500.0f,500.0f)},
			{frand(0.0f,1.0f), frand(0.0f,1.0f), frand(0.0f,1.0f), 1.0f}
			});
	}

	m_debugGui->EndWindow();
}

void MyRender::UpdateControls()
{
	static bool s_controlsOpen = true;
	m_debugGui->BeginWindow(s_controlsOpen, "Controls", {512,512});

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

bool MyRender::RenderFrame()
{	
	// Use imgui as a free blitter!
	if (m_cpuTracer->GetTexture() != nullptr)
	{
		bool stayOpen = true;
		m_debugGui->BeginWindow(stayOpen, "Cpu Output", glm::vec2(c_outputSizeX + 16, c_outputSizeY + 32));
		m_debugGui->Image(*m_cpuTracer->GetTexture(), { c_outputSizeX, c_outputSizeY });
		m_debugGui->EndWindow();
	}

	return true;
}

bool MyRender::Tick()
{
	Core::Timer jobTimer;

	UpdateControls();
	UpdateSceneControls();

	if (!m_isPaused)
	{
		m_cpuTracer->TryDrawScene(m_scene);
	}
	m_cpuTracer->Tick();

	return RenderFrame();
}

void MyRender::Shutdown()
{
	// Shutdown the cpu tracer now as it has handles to graphics resources
	m_cpuTracer = nullptr;
}