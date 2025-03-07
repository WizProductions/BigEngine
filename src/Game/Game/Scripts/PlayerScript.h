#pragma once
#include "Scripts/EntityScript.h"
#include "Systems/HealthSystem.h"

struct PlayerScript final : public EntityScript {

//##############################################################################
//##------------------------------- ATTRIBUTES --------------------------------##
//##############################################################################

private:
	Wiz::InputsManager* m_InputsManager;
	float m_Speed = 10.f;

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################


public:

/*----------< CONSTRUCTORS >----------*/

	explicit PlayerScript(Entity& attachedEntity);
	~PlayerScript() override = default;

/*------------------------------------*/



/* INHERITED FUNCTIONS */

	void Start() override;
	
	void Update() override;
	
	void End() override;
	
	void OnCollision(const CollidingResult result) override;


/* GETTERS */




/* SETTERS */




/* OTHERS FUNCTIONS */

	void Move(Wiz::Key key);

	
};