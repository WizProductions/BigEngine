#pragma once

template <typename... Args>
class EventsManager {

public:
	struct _WRAPPERBASE
    	{
    		virtual ~_WRAPPERBASE() = default;
    		virtual void Invoke(Args... args) = 0;
    	};
    
    	template <typename T>
    	struct _WRAPPER : public _WRAPPERBASE
    	{
    		T* instance;
    		void (T::*method)(Args...);
    		
    		_WRAPPER(T* inst, void (T::*meth)(Args...)) :
    			instance(inst), method(meth)
    		{}
    		
    		void Invoke(Args... args) override
    		{
    			(instance->*method)(std::forward<Args>(args)...);
    		}
    	};

private:

//##############################################################################
//##------------------------------- ATTRIBUTS --------------------------------##
//##############################################################################


private:

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################


public:

/*----------< CONSTRUCTORS >----------*/

/** Constructor */
	EventsManager();
	~EventsManager();

/*------------------------------------*/



/* INHERITED FUNCTIONS */




/* GETTERS */




/* SETTERS */




/* OTHERS FUNCTIONS */

	void Clear();
    int GetCount();

    void Call(Args... args);

    template <typename T>
    void Register(T* pReceiver, void (T::*func)(Args...));

    template <typename T>
    void Unregister(T* pReceiver, void (T::*func)(Args...));
	
	std::vector<_WRAPPERBASE*> m_methods;

	
};

#include "EventsManager.inl"