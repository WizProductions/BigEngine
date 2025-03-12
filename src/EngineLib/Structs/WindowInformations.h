#pragma once

//##############################################################################
//##-------------------------- FORWARD DECLARATIONS --------------------------##
//##############################################################################

//struct X;

//##############################################################################
//##-------------------------------- STRUCTURE -------------------------------##
//##############################################################################

struct WindowInformations {

/*-----------< ATTRIBUTES >-----------*/

	UINT width;
	UINT height;
	HWND m_MainWindowHandle = nullptr; // main window handle
	bool minimized = false;  // is the application minimized?
	bool maximized = false;  // is the application maximized?
	bool resizing = false;   // are the resize bars being dragged?
	bool fullscreen = false;// fullscreen enabled

/*------------------------------------*/


/*----------< CONSTRUCTORS >----------*/
	WindowInformations() = default;
	~WindowInformations() = default;
/*------------------------------------*/

};

//##############################################################################
//##---------------------------- STREAM OPERATOR -----------------------------##
//##############################################################################

std::ostream& operator<<(std::ostream& os, const WindowInformations& _WindowInformations);
