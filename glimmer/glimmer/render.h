#pragma once
#include "core/system.h"
#include "math/glm_headers.h"
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