#include "pch.h"
#include "ThreadsManager.h"

ThreadsManager* ThreadsManager::Get() {
	static ThreadsManager mInstance;
	return &mInstance;
}

HANDLE ThreadsManager::GetHandle(const uint8_t threadID) {
	return m_Threads.at(threadID)->m_Handle;
}


ThreadStruct* ThreadsManager::NewAsyncOperation(void* function, void* param) {

	ThreadStruct* threadStruct = new ThreadStruct;
	
	int threadsSize = static_cast<int>(m_Threads.size());
	threadStruct->m_LogsManager = new LogsManager("Thread_" + std::to_string(threadsSize) + ".logs", "ThreadsLogs");
	threadStruct->m_LogsManager->CreateLogDirectory();
	threadStruct->m_LogsManager->WriteLineToFile("\n\n\n\n\n\n\n", true);
	threadStruct->m_LogsManager->WriteLineToFile("=================================================================================== \n", true);
	threadStruct->m_LogsManager->WriteLineToFile("================== Thread[" + std::to_string(threadsSize) + "] started. ================== \n", true);
	threadStruct->m_LogsManager->WriteLineToFile("=================================================================================== \n", true);
	threadStruct->m_Param = param;
	threadStruct->m_ID = threadsSize;

	threadStruct->m_Handle = (CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(function), (void*)threadStruct, 0, nullptr));
	m_Threads.insert(std::make_pair(threadsSize, threadStruct));

	//TODO better return?? bool?
	return threadStruct;
}

void ThreadsManager::NewAsyncOperation(void* function, ThreadStruct* threadStruct) {

	int threadsSize = static_cast<int>(m_Threads.size());
	threadStruct->m_LogsManager = new LogsManager("Thread_" + std::to_string(threadsSize) + ".logs", "ThreadsLogs");
	threadStruct->m_LogsManager->CreateLogDirectory();
	threadStruct->m_LogsManager->WriteLineToFile("\n\n\n\n\n\n\n", true);
	threadStruct->m_LogsManager->WriteLineToFile("=================================================================================== \n", true);
	threadStruct->m_LogsManager->WriteLineToFile("================== Thread[" + std::to_string(threadsSize) + "] started. ================== \n", true);
	threadStruct->m_LogsManager->WriteLineToFile("=================================================================================== \n", true);
	threadStruct->m_ID = threadsSize;

	threadStruct->m_Handle = (CreateThread(nullptr, 0, static_cast<LPTHREAD_START_ROUTINE>(function), (void*)threadStruct, 0, nullptr));
	m_Threads.insert(std::pair(threadsSize, threadStruct));
}

void ThreadsManager::RemoveThreadStructInMap(const int id) {
	delete m_Threads.at(id); //Delete the threadStruct
	m_Threads.erase(id); //Remove the threadStruct from the map
}
