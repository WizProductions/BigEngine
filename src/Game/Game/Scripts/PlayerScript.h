#pragma once
#include "Scripts/EntityScript.h"

struct PlayerScript final : EntityScript {

//##############################################################################
//##------------------------------- ATTRIBUTES --------------------------------##
//##############################################################################

private:
	Wiz::InputsManager* m_InputsManager;
	float m_Speed = 10.f;
	POINT m_LastMousePos = { 0, 0 }; //TODO: half screen size needed

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
	
	void OnCollision(CollidingResult result) override;


/* GETTERS */




/* SETTERS */




/* OTHERS FUNCTIONS */

	void Move(Wiz::Key key);
	void OnMouseMove(Wiz::Key key);

	
};