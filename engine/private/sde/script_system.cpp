/*
SDLEngine
Matt Hoyle
*/
#include "script_system.h"
#include <sol.hpp>

namespace SDE
{
	ScriptSystem::ScriptSystem()
	{
	}

	ScriptSystem::~ScriptSystem()
	{
	}

	void ScriptSystem::OpenDefaultLibraries(sol::state& state)
	{
		state.open_libraries(sol::lib::base, sol::lib::math);
	}

	void ScriptSystem::RunScript(const char* scriptSource)
	{
		m_globalState->script(scriptSource);
	}

	bool ScriptSystem::RunScript(const char* scriptSource, std::string& errorText)
	{
		try 
		{
			m_globalState->script(scriptSource);
			return true;
		}
		catch (const sol::error& err) 
		{
			errorText = err.what();
			return false;
		}
	}

	bool ScriptSystem::PreInit(Core::ISystemEnumerator& systemEnumerator)
	{
		m_globalState = std::make_unique<sol::state>();
		OpenDefaultLibraries(*m_globalState);
		
		return true;
	}

	void ScriptSystem::Shutdown()
	{
		m_globalState = nullptr;
	}
}