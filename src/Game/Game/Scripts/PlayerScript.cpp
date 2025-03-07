#include "pch.h"
#include "PlayerScript.h"

PlayerScript::PlayerScript(Entity& attachedEntity) :
	EntityScript(attachedEntity)
{}

void PlayerScript::Start() {
	m_InputsManager = &Wiz::InputsManager::Get();
}
void PlayerScript::Update() {

	m_InputsManager->RegisterEventToKey(Wiz::Key::Z, Wiz::KeyState::HELD, [this](const Wiz::Key key) { this->Move(key); });
	m_InputsManager->RegisterEventToKey(Wiz::Key::D, Wiz::KeyState::HELD, [this](const Wiz::Key key) { this->Move(key); });
	m_InputsManager->RegisterEventToKey(Wiz::Key::S, Wiz::KeyState::HELD, [this](const Wiz::Key key) { this->Move(key); });
	m_InputsManager->RegisterEventToKey(Wiz::Key::Q, Wiz::KeyState::HELD, [this](const Wiz::Key key) { this->Move(key); });
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

	auto dc = XMLoadFloat3(&entityTransform.GetPosition());
	directionalVector += XMLoadFloat3(&entityTransform.GetPosition()); //NewVelocity is now the new position (not create another variable for optimization)
	
	XMFLOAT3 newPosition = {};
	XMStoreFloat3(&newPosition, directionalVector);
	
	
	entityTransform.SetPosition(newPosition);
	
}