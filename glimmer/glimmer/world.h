#pragma once
#include <stdint.h>
#include <vector>
#include <sol.hpp>
#include "entity.h"
#include "entity_handle.h"

class World
{
public:
	World() = default;
	~World() = default;
	World(const World& other) = default;

	EntityHandle CreateEntity(const char* name);

	template<class ScriptScope>
	static inline void RegisterScriptType(ScriptScope&);	// registers this type in the scope parameter (either sol::state or sol::table, or your own)
private:
	bool IsNameUnique(const char* s) const;
	std::vector<std::shared_ptr<Entity>> m_allEntities;		// is it fast enough?
};

template<class ScriptScope>
void World::RegisterScriptType(ScriptScope& scope)
{
	scope.new_usertype<World>("World", sol::constructors<World()>(),
		"CreateEntity", &World::CreateEntity
	);
}