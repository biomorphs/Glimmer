#pragma once
#include <sol.hpp>

// Sigh, can't think of a better way right now
// Registers component user type with sol and adds Entity.Create_*c* factory function in lua
#define REGISTER_COMPONENT_SCRIPT_TYPE(scope,c,...)	\
	scope.new_usertype<c>(#c, sol::constructors<c()>(), "GetTypeString", &c::GetTypeString, __VA_ARGS__);	\
	{	\
		auto entityNS = scope["Entity"].get_or_create<sol::table>();	\
		entityNS["Create_" #c] = [](Entity& e)							\
		{																\
			c* cmp = new c();											\
			e.AddComponent(cmp);										\
			return cmp;													\
		};																\
	}

class Component
{
public:
	Component(Component&& other) = default;
	Component(const Component&) = default;
	virtual ~Component() = default;
	virtual const char* GetTypeString() { return "Component"; }

	template<class ScriptScope>
	static inline void RegisterScriptType(ScriptScope&);

protected:
	Component() = default;		// cannot be instantiated
};

// Scripts can still access Component base class members
template<class ScriptScope>
void Component::RegisterScriptType(ScriptScope& scope)
{
	scope.new_usertype<Component>("Component", "GetTypeString", &Component::GetTypeString );
}