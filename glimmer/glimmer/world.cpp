#include "world.h"
#include "kernel/assert.h"

bool World::IsNameUnique(const char* name) const
{
	auto foundIt = std::find_if(m_allEntities.begin(), m_allEntities.end(), [name](const auto& e) {
		return strcmp(name,e->GetName().c_str()) == 0;
	});
	return foundIt == m_allEntities.end();
}

EntityHandle World::CreateEntity(const char* name)
{
	EntityHandle newEntity(nullptr);
	if (IsNameUnique(name))
	{
		auto entity = std::make_shared<Entity>(name);
		m_allEntities.emplace_back(entity);
		return EntityHandle(entity);
	}
	return newEntity;
}