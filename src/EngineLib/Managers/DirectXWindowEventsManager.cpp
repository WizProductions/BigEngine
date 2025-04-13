#include "pch.h"
#include "DirectXWindowEventsManager.h"

DirectXWindowEventsManager::DirectXWindowEventsManager() { this->UnInit(); }
DirectXWindowEventsManager::~DirectXWindowEventsManager() { DESTRUCTOR_UNINIT(m_Initialized); }

bool DirectXWindowEventsManager::Init(ID3D12Device** device) {

	if (m_Initialized)
		return false;

	ApplicationManager& app = *ApplicationManager::Get();
	INIT_PTR(m_TimerPtr, &app.GetTimer());
	INIT_PTR(m_AppIsPausedPtr, &app.AppIsPaused());
	INIT_PTR(m_Device, device);
	INIT_PTR(m_DirectXWindowManager, &DirectXWindowManager::Get());
	INIT_PTR(m_WindowInformationPtr, DirectXWindowManager::m_WindowInformationPtr);

	m_Initialized = true;

	return true;
}

void DirectXWindowEventsManager::UnInit() {

	m_TimerPtr = nullptr;
	m_AppIsPausedPtr = nullptr;
	m_Device = nullptr;
	m_DirectXWindowManager = nullptr;
	m_WindowInformationPtr = nullptr;

	m_Initialized = false;
}

DirectXWindowEventsManager& DirectXWindowEventsManager::Get() {
	static DirectXWindowEventsManager mInstance;
	return mInstance;
}

LRESULT DirectXWindowEventsManager::ProcessWindowMessages(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 0
		// when it becomes active.

	case WM_MOVE:
		m_WindowInformationPtr->UpdateFirstPixelPosition();
		return 0;
		
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			*m_AppIsPausedPtr = true;
			m_TimerPtr->Stop();
		}
		else
		{
			*m_AppIsPausedPtr = false;
			m_TimerPtr->Start();
		}
		return 0;
		
	// WM_SIZE is sent when the user resizes the window.  
	case WM_SIZE:
		// Save the new client area dimensions.
		m_WindowInformationPtr->UpdateWindowSize(LOWORD(lParam), HIWORD(lParam));
		
		if (*m_Device)
		{
			if (wParam == SIZE_MINIMIZED)
			{
				*m_AppIsPausedPtr = true;
				m_WindowInformationPtr->minimized = true;
				m_WindowInformationPtr->maximized = false;
			}
			else if (wParam == SIZE_MAXIMIZED)
			{
				*m_AppIsPausedPtr = false;
				m_WindowInformationPtr->minimized = false;
				m_WindowInformationPtr->maximized = true;
				OnResize();
			}
			else if (wParam == SIZE_RESTORED)
			{

				// Restoring from minimized state?
				if (m_WindowInformationPtr->minimized)
				{
					*m_AppIsPausedPtr = false;
					m_WindowInformationPtr->minimized = false;
					OnResize();
				}

				// Restoring from maximized state?
				else if (m_WindowInformationPtr->maximized)
				{
					*m_AppIsPausedPtr = false;
					m_WindowInformationPtr->maximized = false;
					OnResize();
				}
				else if (m_WindowInformationPtr->resizing)
				{
				}
				else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
				{
					OnResize();
				}
			}
		}
		return 0;

		// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
	case WM_ENTERSIZEMOVE:
		/*mAppPaused = true;
		mResizing  = true;
		mTimer.Stop();*/
		return 0;

		// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
		// Here we reset everything based on the new window dimensions.
	case WM_EXITSIZEMOVE:
		/*mAppPaused = false;
		mResizing  = false;
		mTimer.Start();
		OnResize();*/
		return 0;

		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.x = 200;
		reinterpret_cast<MINMAXINFO*>(lParam)->ptMinTrackSize.y = 200;
		return 0;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		SetCapture(m_WindowInformationPtr->mainWindowHandle);
		return 0;
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
		ReleaseCapture();
		return 0;
	default: std::cerr << "ProcessWindowMessages(): Message not supported! (" << msg << ")" << std::endl;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void DirectXWindowEventsManager::OnResize() {
	m_DirectXWindowManager->OnResize();
}
