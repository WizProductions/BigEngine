#include "pch.h"
#include "CollisionsSystem.h"

CollisionsSystem::CollisionsSystem() { this->UnInit(); }
CollisionsSystem::~CollisionsSystem() { DESTRUCTOR_UNINIT(m_Initialized); }

bool CollisionsSystem::Init() {

	if (m_Initialized)
		return false;
	
	if (!ECSystem::Init())
		return false;

	INIT_PTR(m_SystemManager, &SystemsManager::Get());

	m_SystemType = SystemType::Collisions;
	m_SystemID = SystemId::Collisions;


	m_Initialized = true;
	
	return true;
}
void CollisionsSystem::UnInit() {
	ECSystem::UnInit();

	m_bRevertedCollisionTest = false;
	m_Initialized = false;
}

void CollisionsSystem::Update() {
	const int lastIndex = GameManager::lastEntityIndex;

	FOR_I(0, <=, lastIndex, ++) {

		Entity* entityI = m_Entities[i];
		const CollisionComponent* entityCol = m_SystemManager->GetComponentOfType<CollisionComponent>(*entityI);
		
		if (!entityCol)
			continue;

		if (entityCol->m_CollisionMode == IGNORE)
			continue;

		FOR_J(i+1, <= , lastIndex, ++) {

			Entity* entityJ = m_Entities[j];
			const CollisionComponent* otherEntityCol = m_SystemManager->GetComponentOfType<CollisionComponent>(*entityJ);

			if (!otherEntityCol)
				continue;

			if (otherEntityCol->m_CollisionMode == IGNORE)
				continue;

			const CollidingResult cResult = this->IsColliding<>(*entityCol, *otherEntityCol, *entityI, *entityJ);

			if (cResult.result == true) { //Collided

				//ENTITY-A EVENT
				if (entityCol->m_CollisionMode == BLOCK) {
					auto currentPos = entityI->m_Transform.GetPosition();
					const XMVECTOR newPosVect = XMLoadFloat3(&currentPos) + (XMLoadFloat3(&cResult.normal) * cResult.penetrationDepth); //Rollback
					XMFLOAT3 newPos = { 0.f, 0.f, 0.f };
					XMStoreFloat3(&newPos, newPosVect);

					entityI->m_Transform.SetPosition(newPos);
				}
				
				if (entityI->m_AttachedScript)
					entityI->m_AttachedScript->OnCollision(cResult);

				//REVERSE
				CollidingResult cResultForB = cResult;
				const XMVECTOR reversedCollisionNormalVect = XMVectorNegate(XMLoadFloat3(&cResultForB.normal));;
				XMStoreFloat3(&cResultForB.normal, reversedCollisionNormalVect);
				cResultForB.entityCollided = entityI;

				//ENTITY-B EVENT
				if (otherEntityCol->m_CollisionMode == BLOCK) {
					auto currentPos = entityJ->m_Transform.GetPosition();
					const XMVECTOR newPosVect = XMLoadFloat3(&currentPos) + (reversedCollisionNormalVect * cResultForB.penetrationDepth); //Rollback
					XMFLOAT3 newPos = { 0.f, 0.f, 0.f };
					XMStoreFloat3(&newPos, newPosVect);

					entityJ->m_Transform.SetPosition(newPos);
				}

				if (entityJ->m_AttachedScript)
					entityJ->m_AttachedScript->OnCollision(cResultForB);
			}
		}
	}
}

CollisionsSystem& CollisionsSystem::Get() {
	static CollisionsSystem mInstance;
	return mInstance;
}

AABBCollisionComponent CollisionsSystem::GetTemporaryAABBFromOBB(const OBBCollisionComponent& obb) {
	AABBCollisionComponent aabb;
	aabb.m_RectSize = obb.m_RectSize;
	return aabb; 
}
