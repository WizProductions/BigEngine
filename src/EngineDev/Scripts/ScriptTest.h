#pragma once
#include "EntityScript.h"
#include "Managers/SystemsManager.h"
#include "Entities/Entity.h"
#include "Components/HealthComponent.h"
#include "Systems/HealthSystem.h"

struct ScriptTest final : public EntityScript {

//##############################################################################
//##------------------------------- ATTRIBUTES --------------------------------##
//##############################################################################


private:

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################


public:

/*----------< CONSTRUCTORS >----------*/

	explicit ScriptTest(Entity& attachedEntity) : EntityScript(attachedEntity) {}
	~ScriptTest() override = default;

/*------------------------------------*/



/* INHERITED FUNCTIONS */

	void Start() override {
		std::cout << "begin play" << std::endl;
		HealthSystem::Get().UpdateHealthPoints(*m_AttachedEntity, -5.f);
	}
	void Update() override {
		//std::cout << "update" << std::endl << IS_VALID(SystemsManager::Get().GetComponentOfType<HealthComponent>(*m_AttachedEntity), m_Health) << std::endl;
	}
	void End() override { std::cout << "destroyed" << std::endl; }
	void OnCollision(const CollidingResult result) override { std::cout << result; }


/* GETTERS */




/* SETTERS */




/* OTHERS FUNCTIONS */



	
};
