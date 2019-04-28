/*
SDLEngine
Matt Hoyle
*/
#pragma once

#include "core/system.h"
#include <sol.hpp>		// we expose sol directly, so this doesn't hurt

namespace SDE
{
	class ScriptSystem;

	// Calls scripts that populate the global Config namespace in lua
	// Getters return tables inside the config namespace
	// e.g. GetConfig("Render") gets lua['Config']['Render']
	class ConfigSystem : public Core::ISystem
	{
	public:
		ConfigSystem();
		virtual ~ConfigSystem();

		void LoadConfigFile(const char* path);
		bool PreInit(Core::ISystemEnumerator& systemEnumerator);
		void Shutdown();

		const sol::table& Values() const { return m_configTable; }

	private:
		ScriptSystem* m_scriptSystem;
		sol::table m_configTable;
	};
}