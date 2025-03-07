#pragma once

template <typename... Args>
EventsManager<Args...>::EventsManager() {}

template <typename... Args>
EventsManager<Args...>::~EventsManager() { Clear(); }

template <typename... Args>
void EventsManager<Args...>::Clear()
{
	for ( size_t i=0 ; i<m_methods.size() ; i++ )
		delete m_methods[i];
	m_methods.clear();
}

template <typename... Args>
int EventsManager<Args...>::GetCount()
{
	return (int)m_methods.size();
}

template <typename... Args>
void EventsManager<Args...>::Call(Args... args)
{
	for ( size_t i=0 ; i<m_methods.size() ; i++ )
		m_methods[i]->Invoke(std::forward<Args>(args)...);
}

template <typename... Args>
template <typename T>
void EventsManager<Args...>::Register(T* pReceiver, void (T::*func)(Args...))
{
	m_methods.push_back(new _WRAPPER<T>(pReceiver, func));
}

template <typename... Args>
template <typename T>
void EventsManager<Args...>::Unregister(T* pReceiver, void (T::*func)(Args...))
{
	for ( size_t i=0 ; i<m_methods.size() ; i++ )
	{
		_WRAPPER<T>* pWrapper = dynamic_cast<_WRAPPER<T>*>(m_methods[i]);
		if ( pWrapper && pWrapper->instance==pReceiver && pWrapper->method==func )
		{
			delete m_methods[i];
			m_methods.erase(m_methods.begin() + i);
			--i;
		}
	}
}