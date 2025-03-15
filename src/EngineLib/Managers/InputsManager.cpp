#include "pch.h"
#include "InputsManager.h"

Wiz::InputsManager::InputsManager() { this->UnInit(); }
Wiz::InputsManager::~InputsManager() { DESTRUCTOR_UNINIT(m_Initialized); }
bool Wiz::InputsManager::Init() {

	if (m_Initialized)
		return false;

	//m_KeyEventStructuresTable.fill(KeyEventStructure()); //Reset value but wtf
	m_MouseMoved = false;
	
	m_Initialized = true;
	return true;
}

void Wiz::InputsManager::UnInit() {
	
	m_KeyEventStructuresTable = {};
	m_MouseMoved = false;
	m_Initialized = false;
}

void Wiz::InputsManager::CaptureKey(const Key key) {

	if (!m_Initialized)
		return;
    
	KeyEventStructure& targetKeyEventStruct = this->GetKeyEventStructure(key);
    
	KeyState& lastStateRef = targetKeyEventStruct.m_LastState;
	const short value = GetAsyncKeyState(static_cast<unsigned char>(key));
    
	if (value & 0x8000) { //KeyPressed value is true        
		if (lastStateRef == KeyState::PRESSED) {
			lastStateRef = KeyState::HELD;

			if (targetKeyEventStruct.m_HeldEventPointed)
				targetKeyEventStruct.CallEvent(targetKeyEventStruct.m_PtrToHeldEvent, key);
		}
		else if (lastStateRef == KeyState::HELD) {
			if (targetKeyEventStruct.m_HeldEventPointed)
				targetKeyEventStruct.CallEvent(targetKeyEventStruct.m_PtrToHeldEvent, key);
		}
		else {
			lastStateRef = KeyState::PRESSED;

			if (targetKeyEventStruct.m_PressedEventPointed)
				targetKeyEventStruct.CallEvent(targetKeyEventStruct.m_PtrToPressedEvent, key);
            
			if (targetKeyEventStruct.m_HeldEventPointed)
				targetKeyEventStruct.CallEvent(targetKeyEventStruct.m_PtrToHeldEvent, key);
		}
	}
	else { //KeyPressed value is false
		if (lastStateRef == KeyState::PRESSED || lastStateRef == KeyState::HELD) {
			lastStateRef = KeyState::RELEASED;

			if (targetKeyEventStruct.m_ReleasedEventPointed)
				targetKeyEventStruct.CallEvent(targetKeyEventStruct.m_PtrToReleasedEvent, key);
		}
		else {
			lastStateRef = KeyState::NONE;
		}
	}
}

void Wiz::InputsManager::CheckMouseMoved() {

	POINT previousMousePos = m_LastMousePosition; 
	GetCursorPos(&m_LastMousePosition); //GetMousePosition every frame

	if (previousMousePos.x != m_LastMousePosition.x or previousMousePos.y != m_LastMousePosition.y) {
		KeyEventStructure& mouseKeyEventStructure = GetKeyEventStructure(Key::MOUSE_MOVING);

		m_MouseMoved = true;
		mouseKeyEventStructure.CallEvent(mouseKeyEventStructure.m_PtrToHeldEvent, Key::MOUSE_MOVING);

		return;
	}

	m_MouseMoved = false;
}

void Wiz::InputsManager::CaptureAllKeys() {
	
	for (size_t i(0); i < m_KeyEventStructuresTable.size(); i++) {
		CaptureKey(static_cast<Key>(i));
	}

	CheckMouseMoved(); //Check if mouse moved & store new mouse position
}

Wiz::InputsManager& Wiz::InputsManager::Get() {
	static InputsManager mInstance;
	return mInstance;
}

Wiz::KeyEventStructure& Wiz::InputsManager::GetKeyEventStructure(Key key) {
	const unsigned char index = static_cast<unsigned char>(key);
	if (index >= m_KeyEventStructuresTable.size()) {
		std::cerr << "InputsManager::GetGeyEventStructure(): Key out of range." << std::endl;
		_ASSERT(true);
	}
	
	return m_KeyEventStructuresTable[index];
}

POINT Wiz::InputsManager::GetLastMousePosition(const bool clampedCoord) const {

	WindowInformation* pWindowInformation = DirectXWindowManager::m_WindowInformationPtr;
	if (clampedCoord) {
		const POINT clampedPos = {
			clamp((m_LastMousePosition.x - pWindowInformation->firstPixelPosition.x), 0, pWindowInformation->width),
			clamp((m_LastMousePosition.y - pWindowInformation->firstPixelPosition.y), 0, pWindowInformation->height)
		};

		return clampedPos;
	}
	
	return m_LastMousePosition;
}

bool Wiz::InputsManager::IsPressed(const Key key) {

	const KeyState lastState = GetKeyState(key);
	
	if (lastState == KeyState::PRESSED or lastState == KeyState::HELD)
		return true;

	return false;
}

void Wiz::InputsManager::RegisterEventToKey(const Key key, const KeyState state, const std::function<void(Key)>& ptrToEvent) {

	if (!ptrToEvent) {
		std::cerr << "RegisterEventToKey: Event function is null!" << std::endl;
		return;
	}

	KeyEventStructure& targetKeyEventStruct = GetKeyEventStructure(key);

	if (key == Key::MOUSE_MOVING) { //Mouse (Moving) is a special key
		targetKeyEventStruct.m_PtrToHeldEvent = ptrToEvent;
		targetKeyEventStruct.m_HeldEventPointed = true;

		return;
	}
    
	switch (state) {
	case KeyState::PRESSED:
		targetKeyEventStruct.m_PtrToPressedEvent = ptrToEvent;
		targetKeyEventStruct.m_PressedEventPointed = true;
		break;
	case KeyState::HELD:
		targetKeyEventStruct.m_PtrToHeldEvent = ptrToEvent;
		targetKeyEventStruct.m_HeldEventPointed = true;
		break;
	case KeyState::RELEASED:
		targetKeyEventStruct.m_PtrToReleasedEvent = ptrToEvent;
		targetKeyEventStruct.m_ReleasedEventPointed = true;
		break;

	default:
		std::cerr << "InputsManager::RegisterEventToKey(): Invalid KeyState." << std::endl;
		return;
	}
}

void Wiz::InputsManager::UnRegisterEventToKey(const Key key, const KeyState state) {
	
	KeyEventStructure& targetKeyEventStruct = GetKeyEventStructure(key);

	if (key == Key::MOUSE_MOVING) { //Mouse (Moving) is a special key
		targetKeyEventStruct.m_PtrToHeldEvent = nullptr;
		targetKeyEventStruct.m_HeldEventPointed = false;

		return;
	}
	
	switch (state) {
	case KeyState::PRESSED:
		targetKeyEventStruct.m_PtrToPressedEvent = nullptr;
		targetKeyEventStruct.m_PressedEventPointed = false;
		break;
	case KeyState::HELD:
		targetKeyEventStruct.m_PtrToHeldEvent = nullptr;
		targetKeyEventStruct.m_HeldEventPointed = false;
		break;
	case KeyState::RELEASED:
		targetKeyEventStruct.m_PtrToReleasedEvent = nullptr;
		targetKeyEventStruct.m_ReleasedEventPointed = false;
		break;
		
	default:
		std::cerr << "InputsManager::RegisterEventToKey(): Invalid KeyState." << std::endl;
		return;
	}
	
	return;
}

void Wiz::InputsManager::UnregisterAllEventsToKey(const Key key) {

	KeyEventStructure& targetKeyEventStruct = GetKeyEventStructure(key);

	targetKeyEventStruct.m_PtrToPressedEvent = nullptr;
	targetKeyEventStruct.m_PressedEventPointed = false;

	targetKeyEventStruct.m_PtrToHeldEvent = nullptr;
	targetKeyEventStruct.m_HeldEventPointed = false;

	targetKeyEventStruct.m_PtrToReleasedEvent = nullptr;
	targetKeyEventStruct.m_ReleasedEventPointed = false;

	return;
}

//Map or Array of keys with state
//If 0 become 1 -> Pressed
//If 1 become 1 -> Held
//If 1 become 0 -> Released
//If 0 become 0 -> None

//Function to add in funcPtrArray the function to call when the key is pressed
//Event system

//User need to call this function to add in array and this array call every frame the function when key is a specific state
//Exemple. RegisterEvent(Wiz::Key::A, Pressed, &FunctionToCall);
//Exemple. RegisterEvent(Wiz::Key::A, Held, &FunctionToCall);
//Exemple. RegisterEvent(Wiz::Key::A, Released, &FunctionToCall);
//When Wiz::Key::A is pressed => call the specific function
//Create map? Key:EventCallingStructure

//EventCallingStructure: State:FunctionPtr
//or
//Map: Key:Map:(State:FunctionPtr)

//PreviousCheckedState
//PointerToPressedFunction
//PointerToHeldFunction
//PointerToReleasedFunction
//