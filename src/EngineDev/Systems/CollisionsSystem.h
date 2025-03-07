#pragma once

#include "Systems/ECSystem.h"

struct AABBCollisionComponent;
struct OBBCollisionComponent;
class SystemsManager;

class CollisionsSystem final : public ECSystem {

//##############################################################################
//##------------------------------- ATTRIBUTS --------------------------------##
//##############################################################################

private:
	/* FLAGS */
	bool m_Initialized;
	SystemsManager* m_SystemManager;
	bool m_bRevertedCollisionTest;

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################


public:

/*----------< CONSTRUCTORS >----------*/
	
	CollisionsSystem();
	~CollisionsSystem() override;

/*------------------------------------*/

	bool Init() override;
	void UnInit() override;

/* INHERITED FUNCTIONS */
	
	void Update() override;

/* GETTERS */
	
	static CollisionsSystem& Get();

/* SETTERS */

	
/* OTHERS FUNCTIONS */

	template <typename C1, typename C2>
	CollidingResult IsColliding(const C1& cA, const C2& cB, Entity& entityA, Entity& entityB);
	
	AABBCollisionComponent GetTemporaryAABBFromOBB(const OBBCollisionComponent& obb);
	
};

#include "CollisionsSystem.inl"