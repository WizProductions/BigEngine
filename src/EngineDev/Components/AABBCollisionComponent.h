#pragma once

#include "CollisionComponent.h"

//Forward declarations
struct OBBCollisionComponent;

//Class
struct AABBCollisionComponent final : CollisionComponent {

//##############################################################################
//##------------------------------- ATTRIBUTES --------------------------------##
//##############################################################################

public:
	DirectX::XMFLOAT3 m_RectSize = { 1.f, 1.f, 1.f };

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################

/*----------< CONSTRUCTORS >----------*/

	AABBCollisionComponent() : CollisionComponent() {
		m_CollisionType = AABB;
		m_CollisionMode = IGNORE;
	}
	~AABBCollisionComponent() override = default;

/*------------------------------------*/

	GETTER SystemType GetComponentType() override { return ComponentType; }

	explicit operator OBBCollisionComponent() const;
	explicit operator SphereCollisionComponent&() const;

};
