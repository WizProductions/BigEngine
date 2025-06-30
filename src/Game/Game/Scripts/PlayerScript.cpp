#include "pch.h"
#include "PlayerScript.h"

PlayerScript::PlayerScript(Entity& attachedEntity) :
	EntityScript(attachedEntity), m_InputsManager(nullptr) {}

void PlayerScript::Start() {
	m_InputsManager = &Wiz::InputsManager::Get();
}
void PlayerScript::Update() {

	m_InputsManager->RegisterEventToKey(Wiz::Key::Z, Wiz::KeyState::HELD, [this](const Wiz::Key key) { this->Move(key); });
	m_InputsManager->RegisterEventToKey(Wiz::Key::D, Wiz::KeyState::HELD, [this](const Wiz::Key key) { this->Move(key); });
	m_InputsManager->RegisterEventToKey(Wiz::Key::S, Wiz::KeyState::HELD, [this](const Wiz::Key key) { this->Move(key); });
	m_InputsManager->RegisterEventToKey(Wiz::Key::Q, Wiz::KeyState::HELD, [this](const Wiz::Key key) { this->Move(key); });
	m_InputsManager->RegisterEventToKey(Wiz::Key::MOUSE_MOVING, Wiz::KeyState::HELD, [this](const Wiz::Key key) { this->OnMouseMove(key); });
}

void PlayerScript::End() {
	std::cout << "Player destroyed" << std::endl;
}

void PlayerScript::OnCollision(const CollidingResult result) {
	std::cout << "Colliding ..." << std::endl;
}

void PlayerScript::Move(const Wiz::Key key) {

	XMVECTOR directionalVector = XMVectorZero();
	
	if(key == Wiz::Key::Z) {
		directionalVector = m_AttachedEntity->m_Transform.GetForwardVector();
	}

	if(key == Wiz::Key::D) {
		directionalVector = m_AttachedEntity->m_Transform.GetRightVector();
	}

	if(key == Wiz::Key::Q) {
		directionalVector = m_AttachedEntity->m_Transform.GetLeftVector();
	}

	if(key == Wiz::Key::S) {
		directionalVector = m_AttachedEntity->m_Transform.GetBackwardVector();
	}
	
	Transform& entityTransform = m_AttachedEntity->m_Transform;
	
	directionalVector *= m_Speed; //Add Speed
	directionalVector *= GameManager::Get()->GetDeltaTime(); //Add deltaTime to prevent all configurations (FPS low/high)
	directionalVector += XMLoadFloat3(&entityTransform.GetPosition()); //NewVelocity is now the new position (not create another variable for optimization)
	
	XMFLOAT3 newPosition = {};
	XMStoreFloat3(&newPosition, directionalVector);
	
	
	entityTransform.SetPosition(newPosition);
	
}

void PlayerScript::OnMouseMove(const Wiz::Key key) {
	
	const auto cameraC = CameraSystem::Get().GetSelectedCamera();
	
	if (!cameraC)
		return;

	bool mouseLockedInWindow = DirectXWindowManager::Get().GetLockMouseInWindow();
	POINT lastMousePos = Wiz::InputsManager::Get().GetLastMousePosition();

	lastMousePos.x+= DirectXWindowManager::m_WindowInformationPtr->firstPixelPosition.x;
	lastMousePos.y+= DirectXWindowManager::m_WindowInformationPtr->firstPixelPosition.y;

	if (Wiz::InputsManager::Get().IsPressed(Wiz::Key::MOUSE_LEFT) || mouseLockedInWindow) {
 
		constexpr float sensitivity = 0.025f;

		float dx = sensitivity * static_cast<float>(lastMousePos.x - m_LastMousePos.x);
		float dy = sensitivity * static_cast<float>(lastMousePos.y - m_LastMousePos.y);
		
		/* Y axes reverted */
		XMVECTOR vUp = cameraC->m_Transform.GetUpVector();
		float allUpAxesSum = XMVectorGetX(vUp) + XMVectorGetY(vUp) + XMVectorGetZ(vUp);
		dx *= allUpAxesSum > 0.f ? 1.f : -1.f;
		//TODO: Problem when the sum near 0 because the result switch + => - => + => - ...
    	
		cameraC->m_AttachedEntity->m_Transform.LocalRotate(dy, dx, 0.f);
    	
		std::cout << cameraC->m_Transform.Print(false, true) << std::endl; //{LOG}
	}
	
	else if (Wiz::InputsManager::Get().IsPressed(Wiz::Key::MOUSE_RIGHT)) {

		constexpr float moveSpeed = 0.1f;
		const float deltaMove = moveSpeed * static_cast<float>(lastMousePos.y - m_LastMousePos.y);

		cameraC->m_DistToEntity += deltaMove;
    	
		std::cout << cameraC->m_Transform.Print() << std::endl; //{LOG}
	}

	if (!mouseLockedInWindow) {
		m_LastMousePos = lastMousePos;
	}
	else {
		DirectXWindowManager::m_WindowInformationPtr->UpdateFirstPixelPosition();
		m_LastMousePos.x = DirectXWindowManager::m_WindowInformationPtr->firstPixelPosition.x + DirectXWindowManager::m_WindowInformationPtr->halfWidth;
		m_LastMousePos.y = DirectXWindowManager::m_WindowInformationPtr->firstPixelPosition.y + DirectXWindowManager::m_WindowInformationPtr->halfHeight;
	}
}