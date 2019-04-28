#include "glimmer.h"
#include "kernel/time.h"
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

void Glimmer::SetupSceneScriptBindings()
{
	try
	{
		auto& globals = m_scriptSystem->Globals();
		auto glimmer = globals["glimmer"].get_or_create<sol::table>();
		auto scene = glimmer["scene"].get_or_create<sol::table>();
		scene["addSphere"] = [this](float x, float y, float z, float radius, bool reflect) 
		{
			Material m = reflect ? Material{0.001f, ReflectRefract} : Material{1.0f, Diffuse};
			this->m_scene.spheres.push_back(
				{ { { x, y, z, radius} }, m }
			);
		};
		scene["addPlane"] = [this](float nx, float ny, float nz, float px, float py, float pz, bool reflect) 
		{
			Material m = reflect ? Material{ 0.001f, ReflectRefract } : Material{ 1.0f, Diffuse };
			this->m_scene.planes.push_back(
				{ { {nx,ny,nz}, {px,py,pz} }, m }
			);
		};
		scene["addLight"] = [this](float px, float py, float pz, float r, float g, float b)
		{
			this->m_scene.lights.push_back({ {px,py,pz},{r,g,b} });
		};
		scene["setSkyColour"] = [this](float r, float g, float b)
		{
			this->m_scene.skyColour = glm::vec3(r, g, b);
		};
	}
	catch (const sol::error& err)
	{
		SDE_LOG("Lua error in config.lua - %s", err.what());
	}
}

void Glimmer::CreateScene()
{
	SetupSceneScriptBindings();

	// Load the scene
	std::string errorText;
	if (!m_scriptSystem->RunScriptFromFile("scene.lua", errorText))
	{
		SDE_LOG("Lua error in scene.lua - %s", errorText.data());
	}

	m_sceneDirty = true;
}

void Glimmer::SetupCamera()
{
	m_camera.SetFOVAndAspectRatio(51.52f, (float)c_outputSizeX / (float)c_outputSizeY);
	m_camera.LookAt({ 0.0f, 50.0f, -200.0f }, { 0.0f, 50.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
}

Glimmer::Glimmer()
	: m_debugGui(nullptr)
	, m_scriptSystem(nullptr)
{
	srand((uint32_t)Kernel::Time::HighPerformanceCounterTicks());
}

Glimmer::~Glimmer()
{

}

void Glimmer::InitRenderSystemFromConfig(SDE::RenderSystem& rs)
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

bool Glimmer::PreInit(Core::ISystemEnumerator& systemEnumerator)
{
	auto renderSystem = (SDE::RenderSystem*)systemEnumerator.GetSystem("Render");
	m_debugGui = (DebugGui::DebugGuiSystem*)systemEnumerator.GetSystem("DebugGui");
	m_scriptSystem = (SDE::ScriptSystem*)systemEnumerator.GetSystem("Script");

	// Load config.lua, populates Config global table
	std::string errorText;
	if (!m_scriptSystem->RunScriptFromFile("config.lua", errorText))
	{
		SDE_LOG("Lua error in config.lua - %s", errorText.data());
	}

	// Setup render parameters 
	InitRenderSystemFromConfig(*renderSystem);
	renderSystem->SetClearColour(glm::vec4(0.8f,0.8f,0.8f,1.0f));

	// Set up cpu ray tracer
	CpuRaytracer::Parameters params;
	params.m_jobSystem = (SDE::JobSystem*)systemEnumerator.GetSystem("Jobs");
	params.m_maxRecursion = 6;
	params.m_image.m_dimensions = { c_outputSizeX, c_outputSizeY };
	m_cpuTracer = std::make_unique<CpuRaytracer>(params);

	SetupCamera();

	return true;
}

bool Glimmer::PostInit()
{
	CreateScene();
	return true;
}

void Glimmer::UpdateSceneControls()
{
	static bool s_controlsOpen = true;
	m_debugGui->BeginWindow(s_controlsOpen, "Controls", { 512,512 });

	for (int s = 0; s < m_scene.spheres.size(); ++s)
	{
		char label[256] = { '\0' };
		sprintf_s(label, "Sphere %d", s);
		m_sceneDirty |= m_debugGui->DragVector(label, m_scene.spheres[s].m_sphere.m_posAndRadius, 0.25f);
		sprintf_s(label, "Sphere Material %d", s);
		m_sceneDirty |= m_debugGui->DragFloat(label, m_scene.spheres[s].m_material.m_refractiveIndex, 0.01f, -1.0f, 10.0f);
	}
	if (m_debugGui->Button("Add Sphere"))
	{
		m_scene.spheres.push_back({
			glm::vec4(frand(-50.0f,50.0f), frand(0.0f,50.0f), frand(0.0f,50.0f), frand(2.0, 20.0f)), {1.01f, Diffuse}
			});
		m_sceneDirty = true;
	}

	m_debugGui->Separator();
	for (int l = 0; l < m_scene.lights.size(); ++l)
	{
		char label[256] = { '\0' };
		sprintf_s(label, "Light %d Position", l);
		m_sceneDirty |= m_debugGui->DragVector(label, m_scene.lights[l].m_position, 0.25f);
		sprintf_s(label, "Light %d Colour", l);
		m_sceneDirty |= m_debugGui->DragVector(label, m_scene.lights[l].m_diffuse, 0.05f, 0.0f, 1.0f);
	}
	m_debugGui->Separator();
	glm::vec4 c = glm::vec4(m_scene.skyColour, 1.0f);
	m_sceneDirty |= m_debugGui->ColourEdit("Sky Colour", c);
	m_scene.skyColour = { c.x, c.y, c.z };
	if (m_debugGui->Button("Add Light"))
	{
		m_sceneDirty = true;
		m_scene.lights.push_back({
			{frand(-250.0f,250.0f),frand(50.0f,500.0f), frand(-250.0f,250.0f)},
			{frand(0.0f,0.5f), frand(0.0f,0.5f), frand(0.0f,0.5f)}
			});
	}
	m_debugGui->Separator();
	glm::vec3 cameraPos = m_camera.Position();
	m_sceneDirty |= m_debugGui->DragVector("Camera Position", cameraPos, 0.1f);
	m_camera.SetPosition(cameraPos);

	glm::vec3 cameraT = m_camera.Target();
	m_sceneDirty |= m_debugGui->DragVector("Camera Target", cameraT, 0.1f);
	m_camera.SetTarget(cameraT);

	m_debugGui->EndWindow();
}

void Glimmer::UpdateControls()
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

bool Glimmer::RenderFrame()
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

bool Glimmer::Tick()
{
	UpdateControls();
	UpdateSceneControls();

	if (!m_isPaused && m_sceneDirty)
	{
		m_cpuTracer->TryDrawScene(m_scene, m_camera);
		m_sceneDirty = false;
	}
	m_cpuTracer->Tick();

	return RenderFrame();
}

void Glimmer::Shutdown()
{
	// Shutdown the cpu tracer now as it has handles to graphics resources
	m_cpuTracer = nullptr;
}