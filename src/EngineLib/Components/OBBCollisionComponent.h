#pragma once

#include "CollisionComponent.h"


struct OBBCollisionComponent final : CollisionComponent {

//##############################################################################
//##------------------------------- ATTRIBUTES --------------------------------##
//##############################################################################

public:
	DirectX::XMFLOAT3 m_RectSize = { 1.f, 1.f, 1.f };

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################

/*----------< CONSTRUCTORS >----------*/

	OBBCollisionComponent() : CollisionComponent() {
		m_CollisionType = OBB;
		m_CollisionMode = IGNORE;
	}
	~OBBCollisionComponent() override = default;

/*------------------------------------*/

	SystemType GETTER GetComponentType() override { return ComponentType; }

	explicit operator AABBCollisionComponent() const;
	explicit operator SphereCollisionComponent&() const;
};