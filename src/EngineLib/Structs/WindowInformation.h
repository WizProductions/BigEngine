#pragma once

//##############################################################################
//##-------------------------- FORWARD DECLARATIONS --------------------------##
//##############################################################################

//struct X;

//##############################################################################
//##-------------------------------- STRUCTURE -------------------------------##
//##############################################################################

struct WindowInformation final {

/*-----------< ATTRIBUTES >-----------*/
	LPCWSTR title = L"Engine Window";
	UINT16 width = 720;
	UINT16 halfWidth = 360;
	UINT16 height = 480;
	UINT16 halfHeight = 240;
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	UINT16 graphicsMaxFps = 60;
	HWND mainWindowHandle = nullptr; // main window handle
	bool minimized = false;  // is the application minimized?
	bool maximized = false;  // is the application maximized?
	bool resizing = false;   // are the resize bars being dragged?
	bool fullscreen = false;// fullscreen enabled
	POINT firstPixelPosition = { 0, 0 };
/*------------------------------------*/


/*----------< CONSTRUCTORS >----------*/
	WindowInformation() = default;
	~WindowInformation() = default;
/*------------------------------------*/

	/** Update the window size, half sizes, firstPixelPosition and ratio */
	void UpdateWindowSize(UINT16 _width, UINT16 _height);
	
	void UpdateFirstPixelPosition();

};

//##############################################################################
//##---------------------------- STREAM OPERATOR -----------------------------##
//##############################################################################

std::ostream& operator<<(std::ostream& os, const WindowInformation& windowInformation);
