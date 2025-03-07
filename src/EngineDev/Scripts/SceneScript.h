#pragma once
#include "Script.h"

template <typename Ss>
struct SceneScript : public Script {

//##############################################################################
//##------------------------------- ATTRIBUTES --------------------------------##
//##############################################################################

public:
	Ss* m_AttachedScene = nullptr;

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################


public:

/*----------< CONSTRUCTORS >----------*/

	SceneScript() = default;
	virtual ~SceneScript() = default;

/*------------------------------------*/



/* INHERITED FUNCTIONS */




/* GETTERS */




/* SETTERS */




/* OTHERS FUNCTIONS */

	void Start() override = 0;
	void Update() override = 0;
	void End() override = 0;
	
};
