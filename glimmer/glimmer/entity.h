#pragma once
#include <stdint.h>
#include <string>
#include <sol.hpp>
#include "component.h"

using EntityID = uint32_t;
using EntityName = std::string;		// needs something better

class Entity
{
public:
	Entity();
	Entity(const Entity&);			// note copy generates a new ID
	Entity(Entity&&);
	~Entity();

	explicit Entity(std::string);
	Entity& operator=(Entity&&);
	Entity& operator=(const Entity&);

	static constexpr EntityID InvalidID = -1;
	EntityID GetID() const { return m_id; };

	void SetName(std::string name)		{ m_name = name; }
	std::string GetName() const			{ return m_name; }

	void AddComponent(Component* c);		// takes ownership of the component memory
	uint32_t ComponentCount() const { return (uint32_t)m_components.size(); }
	Component* GetComponentByIndex(int i) const { return m_components[i].get(); }
	Component* GetComponentByType(const char* type);

	template<class ScriptScope>
	static inline void RegisterScriptType(ScriptScope&);	// registers this type in the scope parameter (either sol::state or sol::table, or your own)
private:
	static EntityID GenerateID();

	EntityID m_id;		// id generated automatically
	EntityName m_name;
	std::vector<std::unique_ptr<Component>> m_components;
};

template<class ScriptScope>
void Entity::RegisterScriptType(ScriptScope& scope)
{
	scope.new_usertype<Entity>("Entity", sol::constructors<Entity(), Entity(std::string)>(),
		"GetID", &Entity::GetID,
		"GetName", &Entity::GetName,
		"AddComponent", &Entity::AddComponent,
		"ComponentCount", &Entity::ComponentCount,
		"GetComponentByIndex", &Entity::GetComponentByIndex
	);
}