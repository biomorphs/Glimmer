template<class CallbackFn>
CallbackList<CallbackFn>::CallbackList()
{

}

template<class CallbackFn>
CallbackList<CallbackFn>::~CallbackList()
{

}

template<class CallbackFn>
void CallbackList<CallbackFn>::RegisterCallback(CallbackFn& fn)
{
	m_callbacks.push_back(fn);
}

template <class CallbackFn>
template<class ... ParameterTypes>
void CallbackList<CallbackFn>::operator()(ParameterTypes ... args)
{
	for (auto& it : m_callbacks)
	{
		it(args...);
	}
}