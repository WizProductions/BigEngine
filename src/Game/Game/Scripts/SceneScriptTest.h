#pragma once
#include "Scripts/SceneScript.h"
#include "Game/Scenes/MainScene.h"

struct SceneScriptTest final : SceneScript<MainScene> {

//##############################################################################
//##------------------------------- ATTRIBUTES --------------------------------##
//##############################################################################


//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################


/*----------< CONSTRUCTORS >----------*/

	SceneScriptTest() = default;
	~SceneScriptTest() override = default;

/*------------------------------------*/



/* INHERITED FUNCTIONS */

	void Start() override;
	void Update() override;
	void End() override;


/* GETTERS */




/* SETTERS */




/* OTHERS FUNCTIONS */



	
};