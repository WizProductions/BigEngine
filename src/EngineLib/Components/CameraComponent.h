#pragma once
#include "Component.h"

struct CameraComponent final : Component {
	
    static inline SystemType ComponentType = SystemType::Camera;

	bool m_bStickOnEntity = false;
	float m_DistToEntity = 5.f;
	Entity* m_AttachedEntity = nullptr;
	
    Transform m_Transform = Transform();
	float m_Radius = 0.0f;

    CameraComponent() = default;
    ~CameraComponent() override = default;
	
   GETTER SystemType GetComponentType() override { return ComponentType; }
};