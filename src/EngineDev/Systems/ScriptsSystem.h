#pragma once

#include "Systems/ECSystem.h"

struct Script;

class ScriptsSystem final : public ECSystem {

//##############################################################################
//##------------------------------- ATTRIBUTES -------------------------------##
//##############################################################################

private:
	/* FLAGS */
	bool m_Initialized;

	Script* m_SceneScript;
	
	
//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################


public:

/*----------< CONSTRUCTORS >----------*/
	
	ScriptsSystem();
	~ScriptsSystem() override ;

/*------------------------------------*/

	bool Init() override;
	void UnInit() override;

/* INHERITED FUNCTIONS */

	void Start(Entity& entity) override;
	void Start(Entity** entities, std::list<Component*>** components) override;
	void Update() override;


/* GETTERS */

	static ScriptsSystem& Get();


/* SETTERS */

	SETTER void SetScriptScene(Script* script) { m_SceneScript = script; }


/* OTHERS FUNCTIONS */

	
	
	
};
