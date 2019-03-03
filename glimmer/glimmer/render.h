#pragma once
#include "core/system.h"
#include "math/glm_headers.h"
#include "traceboi.h"
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
	void UpdateControls();
	void UpdateScene();

	std::vector<SceneSphere> m_spheres = {
		{ glm::vec4(0.0f,-48.25f,-56.0f,60.5f), {0.05f} },
		{ glm::vec4(3.25f,-0.75f,-12.0f,2.0f), {0.2f} },
		{ glm::vec4(-3.5f,-1.75f,-11.75f,1.25f), {0.4f} },
		{ glm::vec4(-1.0f,4.25f,-15.25f,4.0f), {0.8f}}
	};
	std::vector<Light> m_lights = {
		{ {-40.0f,100.0f,50.0f}, {0.85f,0.55f,0.25f,1.0f} },
	};
	glm::vec4 m_skyColour = glm::vec4(0.4f,0.42f,0.5f,1.0f);

	std::unique_ptr<Render::Texture> m_outputTexture;
	glm::ivec2 m_windowResolution;
	uint32_t m_forwardPassId;				// forward render pass id
	DebugGui::DebugGuiSystem* m_debugGui;	// imgui interface
	SDE::JobSystem* m_jobSystem;

	enum Trace : int
	{
		Ready = 0,
		InProgress = 1,
		Complete = 2,
		Paused = 3
	};
	std::atomic<int> m_traceStatus;
	std::vector<uint8_t> m_traceResult;
	double m_lastTraceTime;
};