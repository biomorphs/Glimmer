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
	~World();
	World(const World& other) = default;

	void Tick();

	EntityHandle CreateEntity(const char* name);			// does not add it to the world, creation only

	void SpawnEntity(EntityHandle& h);						// spawns the entity in the world, deferred until tick
	void DespawnEntity(EntityHandle& h);					// despawn the entity, deferred until next tick

	template<class ScriptScope>
	static inline void RegisterScriptType(ScriptScope&);	// registers this type in the scope parameter (either sol::state or sol::table, or your own)
private:
	using EntityContainer = std::vector<std::shared_ptr<Entity>>;

	void SpawnPendingEntities();
	void UnspawnPendingEntities();

	EntityContainer::const_iterator FindEntityByName(const char* name, const EntityContainer& ec) const;
	bool IsNameUnique(const char* name, const EntityContainer& ec) const;

	std::vector<std::shared_ptr<Entity>> m_spawnList;		// waiting to spawn on next tick
	std::vector<std::shared_ptr<Entity>> m_unspawnList;		// waiting to unspawn on next tick
	std::vector<std::shared_ptr<Entity>> m_activeList;		// entities active in the world
};

template<class ScriptScope>
void World::RegisterScriptType(ScriptScope& scope)
{
	scope.new_usertype<World>("World", sol::constructors<World()>(),
		"CreateEntity", &World::CreateEntity,
		"SpawnEntity", &World::SpawnEntity,
		"DespawnEntity", &World::DespawnEntity,
		"Tick", &World::Tick
	);
}