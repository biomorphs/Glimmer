#pragma once
#include "core/system.h"
#include "math/glm_headers.h"
#include "render/camera.h"
#include "traceboi.h"
#include "cpu_raytracer.h"
#include <memory>
#include <atomic>
#include <vector>

namespace DebugGui
{
	class DebugGuiSystem;
}

namespace Render
{
	class Texture;
}

namespace SDE
{
	class ScriptSystem;
	class RenderSystem;
}

class MyRender : public Core::ISystem
{
public:
	MyRender();
	virtual ~MyRender();
	virtual bool PreInit(Core::ISystemEnumerator& systemEnumerator);
	virtual bool PostInit();
	virtual bool Tick();
	virtual void Shutdown();
private:
	bool RenderFrame();
	void CreateScene();
	void SetupCamera();

	void UpdateControls();
	void UpdateSceneControls();

	void InitRenderSystemFromConfig(SDE::RenderSystem& rs);
	
	Scene m_scene;
	Render::Camera m_camera;

	bool m_isPaused = false;
	std::unique_ptr<CpuRaytracer> m_cpuTracer;

	uint32_t m_forwardPassId;				// forward render pass id
	DebugGui::DebugGuiSystem* m_debugGui;	// imgui interface
	SDE::ScriptSystem* m_scriptSystem;
};