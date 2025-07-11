#pragma once
#include "Scenes/Scene.h"

class MainScene final : public Scene {

//##############################################################################
//##------------------------------- ATTRIBUTS -------------------------------##
//##############################################################################


	
//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################


public:

/**----------< CONSTRUCTORS >----------*/

	MainScene() = default;
	~MainScene() override = default;

/**------------------------------------*/



/* INHERITED FUNCTIONS */
	
	void Init(GameManager* GameManager) override;
	
	void OnUpdate() override;
	void OnDraw() override;
	void OnEvent() override;

/* GETTERS */




/* SETTERS */




/* OTHERS FUNCTIONS */
	
};