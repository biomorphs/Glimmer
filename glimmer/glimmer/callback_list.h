#pragma once

#include <vector>

// Holds a list of callback objects + handles calling them all with any parameters
template<class CallbackFn>
class CallbackList
{
public:
	CallbackList();
	~CallbackList();

	inline void RegisterCallback(CallbackFn& fn);

	template<class ... ParameterTypes>
	inline void operator()(ParameterTypes ...);		// can handle any arguments so long as they match the fn

private:
	std::vector<CallbackFn> m_callbacks;
};

#include "callback_list.inl"