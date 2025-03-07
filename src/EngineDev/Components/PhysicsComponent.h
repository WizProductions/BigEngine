#pragma once
#include "Component.h"

struct PhysicsComponent final : public Component {

//##############################################################################
//##------------------------------- ATTRIBUTES -------------------------------##
//##############################################################################

public:
	static inline SystemType ComponentType = SystemType::Physics;
	DirectX::XMFLOAT3 m_Velocity = { 0.f, 0.f, 0.f };
	DirectX::XMFLOAT3 m_Acceleration = { 0.f, 0.f, 0.f };
    float m_Mass = 1.f;
    bool m_UseGravity = true;

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################
  
public:

/*----------< CONSTRUCTORS >----------*/

	PhysicsComponent() = default;
	~PhysicsComponent() override =  default;

/*------------------------------------*/

	GETTER virtual SystemType GetComponentType() override { return ComponentType; }
};