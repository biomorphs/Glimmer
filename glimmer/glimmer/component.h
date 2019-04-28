#pragma once
#include <sol.hpp>
#include "entity_handle.h"

// Sigh, can't think of a better way right now
// Defines GetTypeString() for you and binds it to script
// Registers component user type with sol and adds EntityHandle.CreateComponent_*/GetComponent_* in lua
#define REGISTER_COMPONENT_TYPE(scope,c,factory,...)	\
	virtual const char* GetTypeString() { return #c; }			\
	template<class ScriptScope>	\
	static inline void RegisterScriptType(ScriptScope& scope)	\
	{	\
		scope.new_usertype<c>(#c, \
		sol::constructors<c(EntityHandle)>(), "GetTypeString", &c::GetTypeString, __VA_ARGS__);	\
		{	\
			auto entityNS = scope["EntityHandle"].get_or_create<sol::table>();	\
			entityNS["CreateComponent_" #c] = factory;						\
			entityNS["GetComponent_" #c] = [](EntityHandle& e)				\
			{																\
				return (c*)e.GetEntityPtr()->GetComponentByType(#c);		\
			};																\
		}	\
	}

class Component;
using ComponentFactory = std::function<Component*(EntityHandle&)>;

// Default factory used for components with no system
template<class ComponentType>
struct DefaultFactory 
{
	Component* operator()(EntityHandle& e)
	{
		Component* cmp = new ComponentType(e);
		e.GetEntityPtr()->AddComponent(cmp);
		return cmp;
	}		
};

class Component
{
public:
	explicit Component(EntityHandle& e);
	Component(Component&& other) = default;
	Component(const Component&) = default;
	virtual ~Component() = default;
	virtual const char* GetTypeString() { return "Component"; }

	template<class ScriptScope>
	static inline void RegisterScriptType(ScriptScope&);

	EntityHandle GetEntity() const { return EntityHandle(m_parent.lock()); }

protected:
	Component() = default;				// cannot be instantiated
	std::weak_ptr<Entity> m_parent;		// weak ptr so we don't extend lifetime of the parent (the parent owns us)
};

// Scripts can still access Component base class members
template<class ScriptScope>
void Component::RegisterScriptType(ScriptScope& scope)
{
	scope.new_usertype<Component>("Component", "GetTypeString", &Component::GetTypeString );
}