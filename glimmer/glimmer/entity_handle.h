#pragma once
#include <memory>
#include <sol.hpp>

class Entity;

// Active handles keep entities alive!
class EntityHandle
{
public:
	EntityHandle();
	~EntityHandle();
	EntityHandle(const EntityHandle& other);
	EntityHandle(EntityHandle&& other);
	explicit EntityHandle(std::shared_ptr<Entity> e);
	EntityHandle& operator=(const EntityHandle& other);
	EntityHandle& operator=(EntityHandle&& other);

	std::shared_ptr<Entity> GetEntity() { return m_entity; }
	Entity* GetEntityPtr() { return m_entity.get(); }
	bool IsValid() const { return m_entity != nullptr; }

	template<class ScriptScope>
	static inline void RegisterScriptType(ScriptScope&);
private:
	std::shared_ptr<Entity> m_entity;
};

template<class ScriptScope>
void EntityHandle::RegisterScriptType(ScriptScope& scope)
{
	scope.new_usertype<EntityHandle>("EntityHandle", sol::constructors<EntityHandle()>(),
		"GetEntity", &EntityHandle::GetEntityPtr,
		"IsValid", &EntityHandle::IsValid);
}