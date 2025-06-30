#pragma once
#include "Scripts/EntityScript.h"
#include "Systems/HealthSystem.h"

struct ScriptTest final : EntityScript {

//##############################################################################
//##------------------------------- ATTRIBUTES --------------------------------##
//##############################################################################
	

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################
	

/*----------< CONSTRUCTORS >----------*/

	explicit ScriptTest(Entity& attachedEntity) : EntityScript(attachedEntity) {}
	~ScriptTest() override = default;

/*------------------------------------*/



/* INHERITED FUNCTIONS */

	void Start() override {
		std::cout << "begin play" << std::endl;
		HealthSystem::Get().UpdateHealthPoints(*m_AttachedEntity, -5.f);
	}
	
	void Update() override {}
	
	void End() override { std::cout << "destroyed" << std::endl; }
	
	void OnCollision(const CollidingResult result) override {
		std::cout << result;
	}


/* GETTERS */




/* SETTERS */




/* OTHERS FUNCTIONS */



	
};
