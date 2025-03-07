#pragma once

#include "Component.h"

struct CollisionComponent : Component {

//##############################################################################
//##------------------------------- ATTRIBUTES --------------------------------##
//##############################################################################

public:
	static inline SystemType ComponentType = SystemType::Collisions;
	
	CollisionMode m_CollisionMode = IGNORE;
	CollisionType m_CollisionType = UNKNOWN;

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################

/*----------< CONSTRUCTORS >----------*/

	CollisionComponent() = default;
	~CollisionComponent() override = default;

/*------------------------------------*/
	
};