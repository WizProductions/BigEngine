#pragma once
#include "Script.h"

struct Entity;

struct EntityScript : public Script {

//##############################################################################
//##------------------------------- ATTRIBUTES --------------------------------##
//##############################################################################
	
	Entity* m_AttachedEntity = nullptr;

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################


public:

/*----------< CONSTRUCTORS >----------*/

	explicit EntityScript(Entity& attachedEntity);
	~EntityScript() override = default;

/*------------------------------------*/



/* INHERITED FUNCTIONS */




/* GETTERS */




/* SETTERS */




/* OTHERS FUNCTIONS */

	void Start() override = 0;
	void Update() override = 0;
	virtual void OnCollision(const CollidingResult result) {}
	void End() override = 0;

	
};