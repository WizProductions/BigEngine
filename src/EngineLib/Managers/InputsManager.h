#pragma once

namespace Wiz
{
	
//##############################################################################
//##-------------------------------- MACROS ----------------------------------##
//##############################################################################

#ifdef DELETE
#undef DELETE
#endif
	
//##############################################################################
//##----------------------------- ENUMERATIONS -------------------------------##
//##############################################################################

enum class KeyState : char {
	NONE,
	PRESSED,
	HELD,
	RELEASED
};

enum class Key : unsigned char {
	
	// Mouse Buttons
	MOUSE_LEFT = 0x01,
	MOUSE_RIGHT = 0x02,
	MOUSE_MIDDLE = 0x04,

	// Alphabet Keys
	A = 0x41, B = 0x42, C = 0x43, D = 0x44, E = 0x45, F = 0x46,
	G = 0x47, H = 0x48, I = 0x49, J = 0x4A, K = 0x4B, L = 0x4C,
	M = 0x4D, N = 0x4E, O = 0x4F, P = 0x50, Q = 0x51, R = 0x52,
	S = 0x53, T = 0x54, U = 0x55, V = 0x56, W = 0x57, X = 0x58,
	Y = 0x59, Z = 0x5A,

	// Number Keys (Main Keyboard)
	MAIN_0 = 0x30, MAIN_1 = 0x31, MAIN_2 = 0x32, MAIN_3 = 0x33,
	MAIN_4 = 0x34, MAIN_5 = 0x35, MAIN_6 = 0x36, MAIN_7 = 0x37,
	MAIN_8 = 0x38, MAIN_9 = 0x39,

	// Numpad Keys
	NUMPAD_0 = 0x60, NUMPAD_1 = 0x61, NUMPAD_2 = 0x62, NUMPAD_3 = 0x63,
	NUMPAD_4 = 0x64, NUMPAD_5 = 0x65, NUMPAD_6 = 0x66, NUMPAD_7 = 0x67,
	NUMPAD_8 = 0x68, NUMPAD_9 = 0x69,
	NUMPAD_PLUS = 0x6B, NUMPAD_MINUS = 0x6D, NUMPAD_MULTIPLY = 0x6A,
	NUMPAD_DIVIDE = 0x6F, NUMPAD_DECIMAL = 0x6E, NUMPAD_ENTER = 0x0D,

	// Arrow Keys
	ARROW_UP = 0x26, ARROW_DOWN = 0x28, ARROW_LEFT = 0x25, ARROW_RIGHT = 0x27,

	// Function Keys
	F1 = 0x70, F2 = 0x71, F3 = 0x72, F4 = 0x73, F5 = 0x74,
	F6 = 0x75, F7 = 0x76, F8 = 0x77, F9 = 0x78, F10 = 0x79,
	F11 = 0x7A, F12 = 0x7B,

	// Special Keys
	ESCAPE = 0x1B, SPACE = 0x20, TAB = 0x09, ENTER = 0x0D,
	SHIFT = 0x10, CONTROL = 0x11, ALT = 0x12, BACKSPACE = 0x08,
	CAPSLOCK = 0x14, DELETE = 0x2E, END = 0x23, HOME = 0x24,
	INSERT = 0x2D, PAGE_UP = 0x21, PAGE_DOWN = 0x22,
	PRINT_SCREEN = 0x2C, SCROLL_LOCK = 0x91, PAUSE = 0x13,
	NUMLOCK = 0x90,

	// Punctuation and Symbols (Main Keyboard)
	EQUALS = 0xBB, MINUS = 0xBD, MULTIPLY = 0x6A, DIVIDE = 0x6F,
	PERIOD = 0xBE, COMMA = 0xBC, SEMICOLON = 0xBA, QUOTE = 0xDE,
	BACKSLASH = 0xDC, SLASH = 0xBF, LEFT_BRACKET = 0xDB,
	RIGHT_BRACKET = 0xDD, BACKQUOTE = 0xC0, GRAVE = 0xC0,

	// Additional Numpad Keys (for clarification)
	NUMPAD_PERIOD = 0xBE, NUMPAD_COMMA = 0xBC, NUMPAD_SEMICOLON = 0xBA,
	NUMPAD_QUOTE = 0xDE, NUMPAD_SLASH = 0xBF, NUMPAD_BACKSLASH = 0xDC,
};

struct KeyEventStructure {

	KeyEventStructure() = default;

	/* Flags */
	bool m_PressedEventPointed = false;
	bool m_HeldEventPointed = false;
	bool m_ReleasedEventPointed = false;

	/* State */
	KeyState m_LastState = KeyState::NONE;

	/* Event functions */
	std::function<void(Key)> m_PtrToPressedEvent = nullptr;
	std::function<void(Key)> m_PtrToHeldEvent = nullptr;
	std::function<void(Key)> m_PtrToReleasedEvent = nullptr;

	void CallEvent(const std::function<void(Key)>& event, const Key key) {
		if (!event) {
			std::cerr << "InputsManager(): Event function is null, cannot call this event." << std::endl;
			return;
		}
		event(key);
	}
};

class InputsManager {

	//##############################################################################
	//##------------------------------ ATTRIBUTES --------------------------------##
	//##############################################################################

private:

	/* FLAGS */
	bool m_Initialized;

	/* Array of keys with state */
	std::array<KeyEventStructure, 256> m_KeyEventStructuresTable; // o(1) access time

	POINT m_LastMousePosition = {0, 0};

	//#############################################################################
	//##--------------------------------- CLASS ---------------------------------##
	//#############################################################################


public:

	/*----------< CONSTRUCTORS >----------*/

	/** Constructor */
	InputsManager();
	~InputsManager();

	/*------------------------------------*/
	

	/* GETTERS */

	static InputsManager& Get();
	_NODISCARD KeyEventStructure& GetKeyEventStructure(Key key);
	
	/* Get keyState captured this frame */
	GETTER KeyState GetKeyState(const Key key) { return this->GetKeyEventStructure(key).m_LastState; }
	
	_NODISCARD POINT GetLastMousePosition(bool clampedCoord = true) const;
	_NODISCARD bool IsPressed(Key key);
	GETTER bool IsKeyState(const Key key, const KeyState keyState) { return (this->GetKeyState(key) == keyState); }

	
	/* SETTERS */
	
	void RegisterEventToKey(Key key, KeyState state, const std::function<void(Key)>& ptrToEvent);
	void UnRegisterEventToKey(Key key, KeyState state);
	void UnregisterAllEventsToKey(Key key);


	/* OTHERS FUNCTIONS */
	bool Init();
	void UnInit();

	void CaptureKey(Key key);
	void CaptureAllKeys();

};
}