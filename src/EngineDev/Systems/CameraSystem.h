#pragma once
#include "ECSystem.h"
#include "DirectXMath.h"
#include "Components/CameraComponent.h"

class CameraSystem final : public ECSystem {

private:
    bool m_Initialized;
    CameraComponent m_CameraComponent;

public:
    CameraSystem();
    ~CameraSystem() override;

    bool Init() override;
    void UnInit() override;
    void Update() override;

    static CameraSystem& Get();

    void SetCameraPosition(DirectX::XMFLOAT3 position);

    // New methods for input handling
    void HandleInput();
    void MoveCamera(const DirectX::XMFLOAT3& direction);
};
