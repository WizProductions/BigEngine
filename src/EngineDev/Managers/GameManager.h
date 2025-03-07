#pragma once

class SystemsManager;

//Forward declarations
namespace Wiz
{
	class Packet;
	class InputsManager;
}
struct Entity;
class Scene;



class GameManager {

//##############################################################################
//##------------------------------- ATTRIBUTES -------------------------------##
//##############################################################################

private:
	/* FLAGS */
	bool m_Initialized;
	bool m_EntityArrayNeedToBeRealloc;

	/* Pointers to real variables into another classes */
    GameTimer* m_TimerPtr;
    bool* m_AppIsPausedPtr;

public:
	/* Entities array */
	inline static int entityNextID = 0;
	inline static int entityArraySize = 512;
	inline static int lastEntityIndex = -1;
	
private:
	GameTimer* m_Timer;
	
	Scene* m_Scene;
	Wiz::InputsManager* m_InputsManager;
	SystemsManager* m_SystemsManager;

	std::list<Entity*> m_EntitiesPendingSpawn;
	Entity** m_Entities;
	std::list<Entity*> m_EntitiesPendingDestroy;
	

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################


public:

/**----------< CONSTRUCTORS >----------*/

	GameManager();
	~GameManager();

/**------------------------------------*/

	bool Init();
	void UnInit();

/* INHERITED FUNCTIONS */


/* GETTERS */
	
	static GameManager* Get();
	GETTER float GetDeltaTime();
	GETTER Scene* GetScene() const { return m_Scene; }
	/** Return the first element of the entities array, you can get size with GameManager::Get().lastEntityIndex */
	GETTER Entity** GetAllEntities() { return m_Entities; }
	

/* SETTERS */
	
/* OTHERS FUNCTIONS */

	void StartLoop();
	void Update();
	void Draw();
	void HandleInput();

	void AddEntity(Entity* entity);
	void RemoveEntity(Entity* entity);
	void ResetEntities();

	void AsyncExecutePacketInstructions();

	template <typename S, typename Ss>
	S* StartScene();
};

#include "GameManager.inl"