#include "world.h"
#include "kernel/assert.h"
#include <algorithm>

World::~World()
{
}

bool World::IsNameUnique(const char* name, const EntityContainer& ec) const
{
	return FindEntityByName(name, ec) == ec.end();
}

World::EntityContainer::const_iterator World::FindEntityByName(const char* name, const EntityContainer& ec) const
{
	auto foundIt = std::find_if(ec.begin(), ec.end(), [name](const auto& e) {
		return strcmp(name, e->GetName().c_str()) == 0;
	});
	return foundIt;
}

EntityHandle World::CreateEntity(const char* name)
{
	EntityHandle newEntity(nullptr);
	if (IsNameUnique(name, m_spawnList) && IsNameUnique(name, m_activeList))	// Check names early
	{
		auto entity = std::make_shared<Entity>(name);
		return EntityHandle(entity);
	}
	return newEntity;
}

void World::SpawnPendingEntities()
{
	// Take the spawn list in case a callback somewhere triggers more spawns (they will happen next frame) 
	auto spawnThisFrame = std::move(m_spawnList);
	for (const auto& it : spawnThisFrame)
	{
		m_activeList.push_back(it);
		it->OnSpawn();
	}
}

void World::UnspawnPendingEntities()
{
	auto despawnThisFrame = std::move(m_unspawnList);
	for (const auto& it : despawnThisFrame)
	{
		auto& entityToRemove = *it;
		auto toErase = std::remove_if(m_activeList.begin(), m_activeList.end(), 
			[&entityToRemove](const std::shared_ptr<Entity>& e)
			{
				return &(*e) == &(entityToRemove);
			});
		SDE_ASSERT(toErase != m_activeList.end(), "Entity not found in active list?!");
		m_activeList.erase(toErase);

		it->OnUnspawn();
	}
}

void World::DespawnEntity(EntityHandle& h)
{
	m_unspawnList.push_back(h.GetEntity());
}

void World::SpawnEntity(EntityHandle& h)
{
	m_spawnList.push_back(h.GetEntity());
}

void World::Tick()
{
	SpawnPendingEntities();

	UnspawnPendingEntities();
}