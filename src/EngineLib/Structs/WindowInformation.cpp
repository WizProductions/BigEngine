#include "pch.h"
#include "WindowInformation.h"

void WindowInformation::UpdateWindowSize(UINT16 _width, UINT16 _height) {
	width = _width;
	halfWidth = width / 2;
	height = _height;
	halfHeight = height / 2;
	aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	
	UpdateFirstPixelPosition();
}

void WindowInformation::UpdateFirstPixelPosition() {
	firstPixelPosition = { .x= 0, .y= 0};
	if (!ClientToScreen(mainWindowHandle, &firstPixelPosition))
		std::cerr << "WindowInformation::UpdateFirstPixelPosition(): ClientToScreen failed." << std::endl;
}

std::ostream& operator<<(std::ostream& os, const WindowInformation& windowInformation) {

#ifdef _DEBUG
	// os << "==========> WINDOW INFORMATION <===========" << std::endl;
	// os << std::setprecision(2) << std::boolalpha << std::fixed;
	// os << "Collided: " << collidingResult.result << std::endl; // Force true / false format
	// os << "Collision Normal: " << collidingResult.normal << std::endl;
	// os << "EntityCollided: " << collidingResult.entityCollided << " EntityIndex: " << IS_VALID(windowInformation.entityCollided, m_SharedIndex) << std::endl;
	// os << "Collision penetration depth: " << collidingResult.penetrationDepth << std::endl;
	// os << "===========================================" << std::endl;
#endif
	return os;
}
