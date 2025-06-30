#pragma once

//Forward declarations
class GameManager;
struct Script;

class Scene {

//##############################################################################
//##------------------------------- ATTRIBUTES -------------------------------##
//##############################################################################

protected:
	/* FLAGS */
	bool m_Initialized;
	bool m_SceneFinished;

	Script* m_SceneScript;
	
	GameManager* m_MyGameManager;

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################


public:

/**----------< CONSTRUCTORS >----------*/

	Scene();
	virtual ~Scene();

/**------------------------------------*/



/* INHERITED FUNCTIONS */

	virtual void OnUpdate();
	virtual void OnDraw() = 0;
	virtual void OnEvent() = 0;

	virtual void Init(GameManager* GameManager);
	virtual void UnInit();
	virtual void OnSceneEnded();

/* GETTERS */

	GETTER GameManager* GetGameManagerRef() const { return m_MyGameManager; }
	GETTER Script& GetSceneScript() const { return *m_SceneScript; }

/* SETTERS */

	SETTER void SetGameManagerRef(GameManager* GameManagerRef) { m_MyGameManager = GameManagerRef; }
	SETTER void AttachScriptToScene(Script* script) { m_SceneScript = script; }

/* OTHERS FUNCTIONS */


};