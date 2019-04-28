#include "entity.h"
#include "kernel/atomics.h"

Entity::Entity()
{
	m_id = GenerateID();
}

Entity::Entity(Entity&& other)
{
	m_id = other.m_id;
	m_name = std::move(other.m_name);
	other.m_id = InvalidID;
	other.m_name = "";
}

Entity& Entity::operator=(Entity&& other)
{
	m_id = other.m_id;
	m_name = std::move(other.m_name);
	other.m_id = InvalidID;
	other.m_name = "";
	return *this;
}


Entity::Entity(const Entity& other)
{
	m_id = GenerateID();	
	m_name = other.m_name;
}

Entity& Entity::operator=(const Entity& other)
{
	m_id = GenerateID();
	m_name = other.m_name;
	return *this;
}

Entity::Entity(std::string name)
	: Entity()
{
	m_name = name;
}

Entity::~Entity()
{
}

Component* Entity::GetComponentByType(const char* type)
{
	for (const auto& it : m_components)
	{
		if (strcmp(type, it->GetTypeString()) == 0)
		{
			return it.get();
		}
	}
	return nullptr;
}

void Entity::AddComponent(Component* c)
{
	m_components.emplace_back(std::move(c));
}

EntityID Entity::GenerateID()
{
	static Kernel::AtomicInt32 s_idCounter = 0;
	return s_idCounter.Add(1);
}

void Entity::RegisterOnUnspawnCallback(OnUnspawnCallback fn)
{
	m_onUnspawnCb.RegisterCallback(fn);
}

void Entity::RegisterOnSpawnCallback(OnSpawnCallback fn)
{
	m_onSpawnCb.RegisterCallback(fn);
}

void Entity::OnSpawn()
{
	m_onSpawnCb();
}

void Entity::OnUnspawn()
{
	m_onUnspawnCb();
}