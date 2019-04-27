#include "render.h"
#include "kernel/time.h"
#include "kernel/file_io.h"
#include "kernel/log.h"
#include "core/system_enumerator.h"
#include "debug_gui/debug_gui_system.h"
#include "sde/render_system.h"
#include "sde/debug_render.h"
#include "sde/script_system.h"
#include "traceboi.h"
#include <vector>
#include <sol.hpp>

const uint32_t c_outputSizeX = 512;
const uint32_t c_outputSizeY = 512;

float frand(float min, float max)
{
	return min + (rand() / (float)RAND_MAX) * fabs(max - min);
}

void Box(Mesh& m, glm::vec3 origin, glm::vec3 size)
{
	glm::vec3 p0 = origin + glm::vec3( -size.x, -size.y, -size.z );
	glm::vec3 p1 = origin + glm::vec3( size.x, -size.y, -size.z );
	glm::vec3 p2 = origin + glm::vec3( size.x, size.y, -size.z );
	glm::vec3 p3 = origin + glm::vec3( -size.x, size.y, -size.z );
	glm::vec3 p4 = origin + glm::vec3( -size.x, -size.y, size.z );
	glm::vec3 p5 = origin + glm::vec3( size.x, -size.y, size.z );
	glm::vec3 p6 = origin + glm::vec3( size.x, size.y, size.z );
	glm::vec3 p7 = origin + glm::vec3( -size.x, size.y, size.z );

	m.m_triangles.push_back({ p0,p1,p2 });
	m.m_triangles.push_back({ p0,p2,p3 });
	m.m_triangles.push_back({ p4,p6,p5 });
	m.m_triangles.push_back({ p4,p7,p6 });
	m.m_triangles.push_back({ p3, p6, p2 });
	m.m_triangles.push_back({ p3, p7, p6 });
	m.m_triangles.push_back({ p1, p6, p5 });
	m.m_triangles.push_back({ p1, p2, p6 });
}

void MyRender::CreateScene()
{
	for (int i = 0; i < 15; ++i)
	{
		m_scene.spheres.push_back({
			glm::vec4(frand(-100.0f,100.0f), frand(50.0f,100.0f), frand(0.0f,50.0f), frand(2.0, 10.0f)), {0.1f, Diffuse}
			});
	}

	m_scene.planes.push_back({
		{{0.0f,1.0f,0.0f},{0.0f,-100.0f,0.0f}},{0.0f,Diffuse}
		});

	m_scene.spheres.push_back({
		glm::vec4(-35.0f,82.0f,130.0f,60.0f), {0.0f, ReflectRefract}
	});

	///*Mesh testMesh;
	//Box(testMesh, { -50.0f,0.0f,50.0f }, { 32.0f,32.0f,32.0f });
	//testMesh.m_material = { 0.0f,Diffuse };
	//m_scene.meshes.push_back(testMesh);*/

	for (int i = 0; i < 8; ++i)
	{
		m_scene.lights.push_back({
			{frand(-250.0f,250.0f),frand(50.0f,500.0f), frand(-250.0f,250.0f)},
			{frand(0.0f,0.25f), frand(0.0f,0.25f), frand(0.0f,0.25f)}
			});
	}

	m_scene.skyColour = glm::vec3(0.35f, 0.42f, 0.6f);
}

void MyRender::SetupCamera()
{
	m_camera.SetFOVAndAspectRatio(51.52f, (float)c_outputSizeX / (float)c_outputSizeY);
	m_camera.LookAt({ 0.0f, 50.0f, -200.0f }, { 0.0f, 50.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
}

MyRender::MyRender()
	: m_debugGui(nullptr)
	, m_scriptSystem(nullptr)
{
	srand((uint32_t)Kernel::Time::HighPerformanceCounterTicks());
}

MyRender::~MyRender()
{

}

void MyRender::InitRenderSystemFromConfig(SDE::RenderSystem& rs)
{
	// default params
	SDE::RenderSystem::InitialisationParams renderParams(1280, 720, false, "Glimmer");
	try
	{
		auto resolution = m_scriptSystem->Globals()["Config"]["Render"]["Resolution"];
		if (resolution.valid())
		{
			renderParams.m_windowWidth = resolution[1];	// lua table = 1 indexed
			renderParams.m_windowHeight = resolution[2];
		}
		auto fullScreen = m_scriptSystem->Globals()["Config"]["Render"]["Fullscreen"];
		if (fullScreen.valid())
		{
			renderParams.m_fullscreen = fullScreen;
		}
	}
	catch (const sol::error& err)
	{
		SDE_LOG("Lua error in config.lua - %s", err.what());
	}
	rs.SetInitialiseParams(renderParams);
}

bool MyRender::PreInit(Core::ISystemEnumerator& systemEnumerator)
{
	auto inputSystem = (Input::InputSystem*)systemEnumerator.GetSystem("Input");
	auto renderSystem = (SDE::RenderSystem*)systemEnumerator.GetSystem("Render");
	auto jobSystem = (SDE::JobSystem*)systemEnumerator.GetSystem("Jobs");
	m_debugGui = (DebugGui::DebugGuiSystem*)systemEnumerator.GetSystem("DebugGui");
	m_scriptSystem = (SDE::ScriptSystem*)systemEnumerator.GetSystem("Script");

	// Load config.lua, populates Config global table
	std::string configLuaText;
	if (Kernel::FileIO::LoadTextFromFile("config.lua", configLuaText))
	{
		try
		{	
			m_scriptSystem->RunScript(configLuaText.data());
		}
		catch (const sol::error& err)
		{
			SDE_LOG("Lua error in config.lua - %s", err.what());
		}
	}	

	// Setup render parameters 
	InitRenderSystemFromConfig(*renderSystem);

	// Set up render passes
	uint32_t guiPassId = renderSystem->CreatePass("DebugGui");
	renderSystem->GetPass(guiPassId).GetRenderState().m_blendingEnabled = true;
	renderSystem->GetPass(guiPassId).GetRenderState().m_depthTestEnabled = false;
	renderSystem->GetPass(guiPassId).GetRenderState().m_backfaceCullEnabled = false;
	renderSystem->SetClearColour(glm::vec4(0.8f,0.8f,0.8f,1.0f));

	// Set up debug gui
	DebugGui::DebugGuiSystem::InitialisationParams guiParams(renderSystem, inputSystem, guiPassId);
	m_debugGui->SetInitialiseParams(guiParams);

	// Set up cpu ray tracer
	CpuRaytracer::Parameters params;
	params.m_jobCount = 8;
	params.m_jobSystem = jobSystem;
	params.m_maxRecursion = 6;
	params.m_image.m_dimensions = { c_outputSizeX, c_outputSizeY };
	m_cpuTracer = std::make_unique<CpuRaytracer>(params);

	SetupCamera();

	return true;
}

bool MyRender::PostInit()
{
	CreateScene();
	return true;
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
		m_cpuTracer->TryDrawScene(m_scene, m_camera);
	}
	m_cpuTracer->Tick();

	return RenderFrame();
}

void MyRender::Shutdown()
{
	// Shutdown the cpu tracer now as it has handles to graphics resources
	m_cpuTracer = nullptr;
}