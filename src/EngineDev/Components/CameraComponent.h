#pragma once
#include "Component.h"
#include "Maths/Transform.h"

struct CameraComponent final : Component {

public:
    static inline SystemType ComponentType = SystemType::Camera;

    Transform m_Transform = Transform();

    CameraComponent() = default;
    ~CameraComponent() override = default;

    GETTER SystemType GetComponentType() override { return ComponentType; }

    // Methods to get and set position
    DirectX::XMFLOAT3 GetPosition() const { return m_Transform.GetPosition(); }
    void SetPosition(const DirectX::XMFLOAT3& position) { m_Transform.SetPosition(position); }
};
