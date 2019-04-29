#include "glimmer.h"
#include "kernel/time.h"
#include "kernel/log.h"
#include "core/system_enumerator.h"
#include "debug_gui/debug_gui_system.h"
#include "sde/script_system.h"
#include "entity.h"
#include "entity_handle.h"
#include "world.h"
#include <vector>
#include <sol.hpp>

class TestComponent : public Component
{
public:
	explicit TestComponent(EntityHandle& h)
		: Component(h)
	{
		h.GetEntityPtr()->RegisterOnSpawnCallback([this]() { this->OnSpawned(); });
		h.GetEntityPtr()->RegisterOnUnspawnCallback([this]() { this->OnUnspawned(); });
	}
	void DoSomething()
	{
		SDE_LOG("I'm doing something!");
	}
	REGISTER_COMPONENT_TYPE(scope, TestComponent, DefaultFactory<TestComponent>(),
		"DoSomething", &TestComponent::DoSomething);
private:
	void OnSpawned()
	{
		SDE_LOG("Hello! I (%s) just spawned! Look at that. %s!", GetEntity().GetEntityPtr()->GetName().c_str(), m_myBark.c_str());
	}
	void OnUnspawned()
	{
		SDE_LOG("Oh no! I (%s) am unspawning!", GetEntity().GetEntityPtr()->GetName().c_str());
	}
	std::string m_myBark = "woof!";
};

Glimmer::Glimmer()
{
	srand((uint32_t)Kernel::Time::HighPerformanceCounterTicks());
}

Glimmer::~Glimmer()
{

}

bool Glimmer::PreInit(Core::ISystemEnumerator& systemEnumerator)
{
	m_debugGui = (DebugGui::DebugGuiSystem*)systemEnumerator.GetSystem("DebugGui");
	m_scriptSystem = (SDE::ScriptSystem*)systemEnumerator.GetSystem("Script");

	// Test some entity stuff
	Entity::RegisterScriptType(m_scriptSystem->Globals());
	EntityHandle::RegisterScriptType(m_scriptSystem->Globals());
	World::RegisterScriptType(m_scriptSystem->Globals());
	Component::RegisterScriptType(m_scriptSystem->Globals());
	TestComponent::RegisterScriptType(m_scriptSystem->Globals());
	{
		World w;

		m_scriptSystem->Globals()["myWorld"] = &w;		// pass the world by reference

		std::string errorText;
		if (!m_scriptSystem->RunScriptFromFile("entitytests.lua", errorText))
		{
			SDE_LOG("Lua error in entitytests.lua - %s", errorText.c_str());
		}

		m_scriptSystem->Globals()["myWorld"] = nullptr;	// going out of scope now
	}

	return true;
}

bool Glimmer::Tick()
{
	return true;
}

void Glimmer::Shutdown()
{
}