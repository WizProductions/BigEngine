#pragma once

class DirectXWindowEventsManager {

	//##############################################################################
	//##------------------------------- ATTRIBUTES -------------------------------##
	//##############################################################################


private:

	/* FLAGS */
	bool m_Initialized;

	/* Pointers to real variables into another classes */
	GameTimer* m_TimerPtr;
	bool* m_AppIsPausedPtr;
	DirectXWindowManager* m_DirectXWindowManager;
	ID3D12Device** m_Device;
	
	HINSTANCE m_hAppInst = nullptr; // application instance handle

public:

	inline static WindowInformation* m_WindowInformationPtr = nullptr;

private:

	//#############################################################################
	//##--------------------------------- CLASS ---------------------------------##
	//#############################################################################


public:

	/*----------< CONSTRUCTORS >----------*/

	DirectXWindowEventsManager();
	~DirectXWindowEventsManager();

	/*------------------------------------*/

	bool Init(ID3D12Device** device);
	void UnInit();

	/* GETTERS */

	_NODISCARD static DirectXWindowEventsManager& Get();

	/* SETTERS */

	/* OTHERS FUNCTIONS */
	
	static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) { return Get().ProcessWindowMessages(hwnd, msg, wParam, lParam); }
	LRESULT ProcessWindowMessages(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	void OnResize();
};