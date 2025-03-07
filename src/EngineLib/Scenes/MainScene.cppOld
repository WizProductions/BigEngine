
#include "pch.h"
#include "MainScene.h"

#include "Components/SphereCollisionComponent.h"
#include "Managers/GameManager.h"
#include "Entities/Entity.h"
#include "Scripts/ScriptTest.h"
#include "Components/HealthComponent.h"
#include "Systems/CollisionsSystem.h"

//##############################################################################
//################################ CONSTRUCTORS ################################
//##############################################################################

//Not need another constructor now

//##############################################################################
//################################## METHODS ###################################
//##############################################################################

void MainScene::Init(GameManager* GameManager)
{
	Scene::Init(GameManager);
	std::cout << "[DEBUG] Début de MainScene::Init()" << std::endl;

	Entity* e = new Entity();
	new ScriptTest(*e); //Add script to entity (and set it to the entity)
	SystemsManager::Get().AddComponent<HealthComponent>(*e);
	auto ccc = SystemsManager::Get().AddComponent<SphereCollisionComponent>(*e);
	m_MyGameManager->AddEntity(e);

	Entity* eb = new Entity();
	new ScriptTest(*eb);
	m_MyGameManager->AddEntity(eb);
	auto cccb = SystemsManager::Get().AddComponent<AABBCollisionComponent>(*eb);

	ccc->m_CollisionMode = OVERLAP;
	cccb->m_CollisionMode = OVERLAP;

	//   -A-
	ccc->m_Radius = 50.f;
	//ccc->m_RectSize = { 100.f, 100.f, 100.f };
	e->m_transform.SetPosition({ 0.f, 0.f, 590.f });

	//   -B-
	
	//cccb->m_Radius = 50.f;
	cccb->m_RectSize = { 100.f, 100.f, 100.f };
	eb->m_transform.SetPosition({ 0.f, 0.f, 610.f });

	//OBB
	e->m_transform.SetLocalRotation(0, 20, 0);
	eb->m_transform.SetLocalRotation(0, -20, 0);

	std::cout << eb->m_transform;
//#define _revert_test_
#ifndef _revert_test_
	const auto ac = CollisionsSystem::Get().IsColliding(*ccc, *cccb, *e, *eb);
#else
	const auto ac = CollisionsSystem::Get().IsColliding(*cccb, *ccc, *eb, *e);
#endif
	
	std::cout << ac;
}


void MainScene::OnUpdate() {
	Scene::OnUpdate();
}

void MainScene::OnDraw() {

}

void MainScene::OnEvent() {
	
}