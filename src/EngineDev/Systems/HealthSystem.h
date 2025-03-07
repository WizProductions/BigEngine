#pragma once

#include "Systems/ECSystem.h"

class HealthSystem final : public ECSystem {

//##############################################################################
//##------------------------------- ATTRIBUTES --------------------------------##
//##############################################################################

private:
	/* FLAGS */
	bool m_Initialized;

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################


public:

/*----------< CONSTRUCTORS >----------*/

	HealthSystem();
	~HealthSystem() override;

/*------------------------------------*/

	bool Init() override;
	void UnInit() override;

/* INHERITED FUNCTIONS */

	void Update() override;

/* GETTERS */

	static HealthSystem& Get();
	_NODISCARD float GetHealthPoints(const Entity& entity) const;


/* SETTERS */

	/** Force set the health points of the entity, can exceed max healthPoints */
	void SetHealthPoints(const Entity& entity, float healthPoints);


/* OTHERS FUNCTIONS */

	/** Add health points to the entity */
	bool UpdateHealthPoints(const Entity& entity, float delta);
	
};