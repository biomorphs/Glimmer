#pragma once
#include <sol.hpp>
#include "serialisation.h"
#include "entity_handle.h"

// Component Base Class
// To add a new component (e.g. YourComponentType) you MUST:
//	- add REGISTER_COMPONENT_TYPE(YourComponentType) to the class declaration
//	- call YourComponentType::RegisterScriptType with a script context
//		this registers factory + getter for entity handle in scripts
//			EntityHandle:GetComponent_YourComponentType
//			EntityHandle::CreateComponent_YourComponentType
class Component
{
public:
	explicit Component(EntityHandle& e);
	Component(Component&& other) = default;
	Component(const Component&) = default;
	virtual ~Component() = default;
	virtual const char* GetTypeString() const { return "Component"; }

	template<class ScriptScope>
	static inline void RegisterScriptType(ScriptScope&);

	EntityHandle GetEntity() const { return EntityHandle(m_parent.lock()); }

	SDE_SERIALISED_CLASS();

protected:
	Component() = default;				// cannot be instantiated
	std::weak_ptr<Entity> m_parent;		// weak ptr so we don't extend lifetime of the parent (the parent owns us)
};

SDE_SERIALISE_BEGIN(Component)
SDE_SERIALISE_PROPERTY("Type", GetTypeString());
SDE_SERIALISE_END()

// Default factory used for components with no system
template<class ComponentType>
struct DefaultFactory
{
	class Component* operator()(EntityHandle& e)
	{
		class Component* cmp = new ComponentType(e);
		e.GetEntityPtr()->AddComponent(cmp);
		return cmp;
	}
};

// Sigh, can't think of a better way right now
// Defines GetTypeString() for you and binds it to script
// Registers component user type with sol and adds EntityHandle.CreateComponent_*/GetComponent_* in lua
#define REGISTER_COMPONENT_TYPE(scope,c,factory,...)	\
	virtual const char* GetTypeString() const { return #c; }			\
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
				return (c*)e->GetComponentByType(#c);		\
			};																\
		}	\
	}

// Scripts can still access Component base class members
template<class ScriptScope>
void Component::RegisterScriptType(ScriptScope& scope)
{
	scope.new_usertype<Component>("Component", "GetTypeString", &Component::GetTypeString );
}