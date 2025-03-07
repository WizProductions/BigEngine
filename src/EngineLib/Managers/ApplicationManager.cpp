#include "pch.h"
#include "ApplicationManager.h"

ApplicationManager* ApplicationManager::Get() {
	static ApplicationManager mInstance;
	return &mInstance;
}

bool ApplicationManager::Init() {

	if (m_Initialized)
		return false;

	m_Timer = GameTimer();

	INIT_PTR(m_DirectXWindowManager, &DirectXWindowManager::Get());
	m_DirectXWindowManager->Init(720, 480, L"Engine");

	m_Initialized = true;
	return true;
}

void ApplicationManager::UnInit() {

	m_AppIsPaused = false;
	m_DirectXWindowManager = nullptr;
	m_Initialized = false;
}

void ApplicationManager::AddConsole() {

#ifdef _DEBUG
    AllocConsole();
    FILE* file;
    freopen_s(&file, "CONOUT$", "w", stdout); //cout -> console
    freopen_s(&file, "CONIN$", "r", stdin);  // cin -> console
#endif
}