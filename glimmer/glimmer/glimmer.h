#pragma once
#include "core/system.h"

namespace DebugGui
{
	class DebugGuiSystem;
}

namespace SDE
{
	class ScriptSystem;
}

class Glimmer : public Core::ISystem
{
public:
	Glimmer();
	virtual ~Glimmer();
	virtual bool PreInit(Core::ISystemEnumerator& systemEnumerator);
	virtual bool Tick();
	virtual void Shutdown();
private:
	DebugGui::DebugGuiSystem* m_debugGui = nullptr;
	SDE::ScriptSystem* m_scriptSystem = nullptr;
};