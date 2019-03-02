#pragma once
#include "core/system.h"
#include "math/glm_headers.h"

namespace DebugGui
{
	class DebugGuiSystem;
}

class MyRender : public Core::ISystem
{
public:
	MyRender(int windowResX, int windowResY);
	virtual ~MyRender();
	virtual bool PreInit(Core::ISystemEnumerator& systemEnumerator);
	virtual bool Tick();
	virtual void Shutdown();
private:
	glm::ivec2 m_windowResolution;
	uint32_t m_forwardPassId;				// forward render pass id
	DebugGui::DebugGuiSystem* m_debugGui;	// imgui interface
};