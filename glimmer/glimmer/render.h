#pragma once
#include "core/system.h"
#include "math/glm_headers.h"
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
	class JobSystem;
}

class MyRender : public Core::ISystem
{
public:
	MyRender(int windowResX, int windowResY);
	virtual ~MyRender();
	virtual bool PreInit(Core::ISystemEnumerator& systemEnumerator);
	virtual bool PostInit();
	virtual bool Tick();
	virtual void Shutdown();
private:
	bool RenderFrame();
	void CreateScene();

	void UpdateControls();
	void UpdateSceneControls();
	
	Scene m_scene;
	bool m_isPaused = false;

	std::unique_ptr<CpuRaytracer> m_cpuTracer;

	glm::ivec2 m_windowResolution;			// 
	uint32_t m_forwardPassId;				// forward render pass id
	DebugGui::DebugGuiSystem* m_debugGui;	// imgui interface
};