#pragma once

#include "CollisionComponent.h"

struct OBBCollisionComponent;
struct AABBCollisionComponent;

struct SphereCollisionComponent final : CollisionComponent {

//##############################################################################
//##------------------------------- ATTRIBUTES --------------------------------##
//##############################################################################

public:
	float m_Radius = 1.f;

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################

/*----------< CONSTRUCTORS >----------*/

	SphereCollisionComponent() : CollisionComponent() {
		m_CollisionType = SPHERE;
		m_CollisionMode = IGNORE;
	}
	~SphereCollisionComponent() override = default;

/*------------------------------------*/

	GETTER SystemType GetComponentType() override { return ComponentType; }

	explicit operator AABBCollisionComponent&() const;
	explicit operator OBBCollisionComponent&() const ;
	
};
