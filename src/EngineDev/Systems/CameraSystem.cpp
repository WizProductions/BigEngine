#include "pch.h"
#include "CameraSystem.h"
#include <iostream>

CameraSystem::CameraSystem() { this->UnInit(); }
CameraSystem::~CameraSystem() { DESTRUCTOR_UNINIT(m_Initialized); }

bool CameraSystem::Init() {
    ECSystem::Init();

    if (m_Initialized) {
        return false;
    }

    m_SystemType = SystemType::Scripts;
    m_Initialized = true;

    return true;
}

void CameraSystem::UnInit() {
    ECSystem::UnInit();
    m_Initialized = false;
}

void CameraSystem::Update() {
    std::cout << "CameraSystem Update" << std::endl;
    HandleInput();
}

CameraSystem& CameraSystem::Get() {
    static CameraSystem m_Instance;
    return m_Instance;
}

void CameraSystem::SetCameraPosition(DirectX::XMFLOAT3 position) {
    m_CameraComponent.SetPosition(position);
}

void CameraSystem::HandleInput() {
    bool moveForward = false; // Replace with actual input check for 'z'
    bool moveBackward = false; // Replace with actual input check for 's'
    bool moveLeft = false; // Replace with actual input check for 'q'
    bool moveRight = false; // Replace with actual input check for 'd'

    DirectX::XMFLOAT3 direction = { 0.0f, 0.0f, 0.0f };

    if (moveForward) {
        direction.z += 1.0f;
    }
    if (moveBackward) {
        direction.z -= 1.0f;
    }
    if (moveLeft) {
        direction.x -= 1.0f;
    }
    if (moveRight) {
        direction.x += 1.0f;
    }

    MoveCamera(direction);
}

void CameraSystem::MoveCamera(const DirectX::XMFLOAT3& direction) {
    DirectX::XMFLOAT3 currentPosition = m_CameraComponent.GetPosition();
    currentPosition.x += direction.x;
    currentPosition.y += direction.y;
    currentPosition.z += direction.z;
    m_CameraComponent.SetPosition(currentPosition);
}
