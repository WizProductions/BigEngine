#pragma once

//##############################################################################
//##-------------------------- FORWARD DECLARATIONS --------------------------##
//##############################################################################

struct ThreadStruct {

	explicit ThreadStruct() = default;
	explicit ThreadStruct(const HANDLE handle, LogsManager* logsManager, const int id) :
		m_Handle(handle)
		, m_LogsManager(logsManager)
		, m_ID(id)
	{};
	~ThreadStruct() { delete m_LogsManager; }

	HANDLE m_Handle;
	LogsManager* m_LogsManager;
	int m_ID;
	void* m_Param;
};


class ThreadsManager {

//##############################################################################
//##------------------------------- ATTRIBUTES -------------------------------##
//##############################################################################

private:
	std::map<int, ThreadStruct*> m_Threads;

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################


public:

/**----------< CONSTRUCTORS >----------*/

	ThreadsManager() = default;
	~ThreadsManager() = default;

/**------------------------------------*/



/* INHERITED FUNCTIONS */
	

/* GETTERS */

	static ThreadsManager* Get();
	HANDLE GetHandle(uint8_t threadID);

/* SETTERS */


/* OTHERS FUNCTIONS */

	/** Create a new async operation and returns id of this thread in the list. */
	ThreadStruct* NewAsyncOperation(void* function, void* param);
	void NewAsyncOperation(void* function, ThreadStruct* threadStruct);
	void RemoveThreadStructInMap(int id);	
};