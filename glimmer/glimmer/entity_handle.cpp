#include "entity_handle.h"

EntityHandle::EntityHandle()
{
}

EntityHandle::EntityHandle(std::shared_ptr<Entity> e)
	: m_entity(e)
{
}

EntityHandle::~EntityHandle()
{
	m_entity.reset();
}

EntityHandle::EntityHandle(const EntityHandle& other)
{
	m_entity = other.m_entity;
}

EntityHandle::EntityHandle(EntityHandle&& other)
{
	m_entity = std::move(other.m_entity);
}

EntityHandle& EntityHandle::operator=(const EntityHandle& other)
{
	m_entity = other.m_entity;
	return *this;
}

EntityHandle& EntityHandle::operator=(EntityHandle&& other)
{
	m_entity = std::move(other.m_entity);
	return *this;
}