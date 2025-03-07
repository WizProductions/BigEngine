#include "pch.h"
#include "PhysicsSystem.h"
#include "Components/PhysicsComponent.h"
#include <iostream>
#include "Managers/SystemsManager.h"


PhysicsSystem::PhysicsSystem() { this->UnInit(); }
PhysicsSystem::~PhysicsSystem() { DESTRUCTOR_UNINIT(m_Initialized); }

bool PhysicsSystem::Init() {

	ECSystem::Init();
	
	if (m_Initialized)
		return false;

	m_SystemType = SystemType::Physics;
	
	m_Initialized = true;

	return true;
}

void PhysicsSystem::UnInit() {
	
	m_Gravity = {0.0f, -9.81f, 0.0f};
	m_FixedTimeStep = 1.0f / 60.0f;

	m_Initialized = false;
}

void PhysicsSystem::Update() {
	std::cout << "PhysicsSystem Update" << std::endl;
	//entity.GetComponentOfType();
}

PhysicsSystem& PhysicsSystem::Get() {
	static PhysicsSystem mInstance;
	return mInstance;
}

void PhysicsSystem::ApplyForce(const DirectX::XMFLOAT3& force) {
	DirectX::XMVECTOR vForce = DirectX::XMLoadFloat3(&force);
	DirectX::XMVECTOR vAccel = DirectX::XMLoadFloat3(&m_Acceleration);
	vAccel = DirectX::XMVectorAdd(vAccel, vForce);

	DirectX::XMStoreFloat3(&m_Acceleration, vAccel);
	std::cout << "PhysicsSystem ApplyForce" << std::endl;

}