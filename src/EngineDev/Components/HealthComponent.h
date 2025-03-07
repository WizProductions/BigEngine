#pragma once
#include "Component.h"

struct HealthComponent final : Component {

//##############################################################################
//##------------------------------- ATTRIBUTES --------------------------------##
//##############################################################################

public:
	static inline SystemType ComponentType = SystemType::Health;

	float m_Health = 20.f;
	float m_MaxHealth = 20.f;
	bool isDead = false;

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################

/*----------< CONSTRUCTORS >----------*/

	HealthComponent() = default;
	~HealthComponent() override = default;

/*------------------------------------*/

	GETTER virtual SystemType GetComponentType() override { return ComponentType; }
	
};