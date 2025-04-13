#include "pch.h"
#include "MainScene.h"
#include "Entities/Entity.h"
#include "Game/Scripts/PlayerScript.h"

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

	/* PLAYER */
	Entity* player = new Entity();
	new PlayerScript(*player); //Add script to entity (and set it to the entity)
	SystemsManager::Get().AddComponent<HealthComponent>(*player);
	//auto ccc = SystemsManager::Get().AddComponent<AABBCollisionComponent>(*player);
	m_MyGameManager->AddEntity(player);

//Player Cam
const auto cameraC = SystemsManager::Get().AddComponent<CameraComponent>(*player);
cameraC->m_DistToEntity = 5.f;
cameraC->m_bStickOnEntity = true;
CameraSystem::Get().SetSelectedCamera(cameraC);
player->m_Transform.SetPosition(850, 2731, 1300);
player->m_Transform.SetScale(0.25, 0.25, 0.25);

//Others
Entity* ground = new Entity();
m_MyGameManager->AddEntity(ground);
ground->m_Transform.SetPosition(850, 2730, 1300);
ground->m_Transform.SetScale(20, 0.25, 20);

Entity* Pyramid1 = new Entity();
m_MyGameManager->AddEntity(Pyramid1);
Pyramid1->m_Transform.SetPosition(850, 2731, 1303);
Pyramid1->m_Transform.SetScale(.35, .35, .35);

Entity* Pyramid2 = new Entity();
m_MyGameManager->AddEntity(Pyramid2);
Pyramid2->m_Transform.SetPosition(848, 2731, 1303);
Pyramid2->m_Transform.SetScale(.35, .35, .35);

Entity* cube = new Entity();
m_MyGameManager->AddEntity(cube);
cube->m_Transform.SetPosition(852, 2731, 1303);
cube->m_Transform.SetScale(.35, .35, .35);

Entity* building0 = new Entity();
m_MyGameManager->AddEntity(building0);
building0->m_Transform.SetPosition(852, 2750, 1350);
building0->m_Transform.SetScale(3, 100, 3);

Entity* building1 = new Entity();
m_MyGameManager->AddEntity(building1);
building1->m_Transform.SetPosition(700, 2750, 1330);
building1->m_Transform.SetScale(3, 100, 3);

Entity* building2 = new Entity();
m_MyGameManager->AddEntity(building2);
building2->m_Transform.SetPosition(948, 2750, 1330);
building2->m_Transform.SetScale(3, 100, 3);
	
Entity* building3 = new Entity();
m_MyGameManager->AddEntity(building3);
building3->m_Transform.SetPosition(780, 2750, 1400);
building3->m_Transform.SetScale(4, 80, 4);

Entity* building4 = new Entity();
m_MyGameManager->AddEntity(building4);
building4->m_Transform.SetPosition(920, 2750, 1420);
building4->m_Transform.SetScale(3.5, 90, 3.5);

Entity* building5 = new Entity();
m_MyGameManager->AddEntity(building5);
building5->m_Transform.SetPosition(800, 2750, 1150);
building5->m_Transform.SetScale(5, 120, 5);

Entity* building6 = new Entity();
m_MyGameManager->AddEntity(building6);
building6->m_Transform.SetPosition(900, 2750, 1200);
building6->m_Transform.SetScale(4, 110, 4);

Entity* building7 = new Entity();
m_MyGameManager->AddEntity(building7);
building7->m_Transform.SetPosition(750, 2750, 1250);
building7->m_Transform.SetScale(3, 85, 3);

Entity* building8 = new Entity();
m_MyGameManager->AddEntity(building8);
building8->m_Transform.SetPosition(650, 2750, 1200);
building8->m_Transform.SetScale(4.5, 95, 4.5);

Entity* building9 = new Entity();
m_MyGameManager->AddEntity(building9);
building9->m_Transform.SetPosition(1000, 2750, 1250);
building9->m_Transform.SetScale(3.8, 105, 3.8);

Entity* building10 = new Entity();
m_MyGameManager->AddEntity(building10);
building10->m_Transform.SetPosition(1050, 2750, 1350);
building10->m_Transform.SetScale(4.2, 115, 4.2);

Entity* building11 = new Entity();
m_MyGameManager->AddEntity(building11);
building11->m_Transform.SetPosition(730, 2750, 1450);
building11->m_Transform.SetScale(3.2, 75, 3.2);

Entity* building12 = new Entity();
m_MyGameManager->AddEntity(building12);
building12->m_Transform.SetPosition(850, 2750, 1500);
building12->m_Transform.SetScale(5, 130, 5);

Entity* building13 = new Entity();
m_MyGameManager->AddEntity(building13);
building13->m_Transform.SetPosition(950, 2750, 1100);
building13->m_Transform.SetScale(3.7, 85, 3.7);

Entity* building14 = new Entity();
m_MyGameManager->AddEntity(building14);
building14->m_Transform.SetPosition(600, 2750, 1300);
building14->m_Transform.SetScale(4.8, 125, 4.8);

Entity* building15 = new Entity();
m_MyGameManager->AddEntity(building15);
building15->m_Transform.SetPosition(680, 2750, 1380);
building15->m_Transform.SetScale(3.5, 95, 3.5);

Entity* building16 = new Entity();
m_MyGameManager->AddEntity(building16);
building16->m_Transform.SetPosition(980, 2750, 1380);
building16->m_Transform.SetScale(4, 110, 4);

Entity* building17 = new Entity();
m_MyGameManager->AddEntity(building17);
building17->m_Transform.SetPosition(880, 2750, 1050);
building17->m_Transform.SetScale(3.3, 90, 3.3);

#ifdef _TESTLAG

	Entity* lagBuilding[50];
	const float maxRadius = 1500.0f; 

	for (int i = 0; i < 100; i++) {
		lagBuilding[i] = new Entity();
		m_MyGameManager->AddEntity(lagBuilding[i]);
    

		float offsetX = (((i * 17) % 600) - 300) * (maxRadius / 300.0f);
		float offsetZ = (((i * 23) % 600) - 300) * (maxRadius / 300.0f);
    

		if (abs(offsetX) > maxRadius) offsetX = (offsetX > 0) ? maxRadius * 0.97f : -maxRadius * 0.97f;
		if (abs(offsetZ) > maxRadius) offsetZ = (offsetZ > 0) ? maxRadius * 0.97f : -maxRadius * 0.97f;
    
		lagBuilding[i]->m_Transform.SetPosition(
			850 + offsetX,
			2750,
			1300 + offsetZ
		);
    

		float width = 3.0f + (i % 5);
		float height = 70.0f + (i % 7) * 10;
		float depth = 3.0f + (i % 5);
    
		lagBuilding[i]->m_Transform.SetScale(width, height, depth);
    

		DirectXWindowManager::Get().DrawEntity("box", *lagBuilding[i]);
	}
#endif


DirectXWindowManager::Get().DrawEntity("box", *player);
DirectXWindowManager::Get().DrawEntity("box", *ground);
DirectXWindowManager::Get().DrawEntity("prisme_triangle", *Pyramid1);
DirectXWindowManager::Get().DrawEntity("prisme_triangle_carre", *Pyramid2);
DirectXWindowManager::Get().DrawEntity("box", *cube);
DirectXWindowManager::Get().DrawEntity("box", *building0);
DirectXWindowManager::Get().DrawEntity("box", *building1);
DirectXWindowManager::Get().DrawEntity("box", *building2);
DirectXWindowManager::Get().DrawEntity("box", *building3);
DirectXWindowManager::Get().DrawEntity("box", *building4);
DirectXWindowManager::Get().DrawEntity("box", *building5);
DirectXWindowManager::Get().DrawEntity("box", *building6);
DirectXWindowManager::Get().DrawEntity("box", *building7);
DirectXWindowManager::Get().DrawEntity("box", *building8);
DirectXWindowManager::Get().DrawEntity("box", *building9);
DirectXWindowManager::Get().DrawEntity("box", *building10);
DirectXWindowManager::Get().DrawEntity("box", *building11);
DirectXWindowManager::Get().DrawEntity("box", *building12);
DirectXWindowManager::Get().DrawEntity("box", *building13);
DirectXWindowManager::Get().DrawEntity("box", *building14);
DirectXWindowManager::Get().DrawEntity("box", *building15);
DirectXWindowManager::Get().DrawEntity("box", *building16);
DirectXWindowManager::Get().DrawEntity("box", *building17);

//#define _revert_test_
#ifndef _revert_test_
	//const auto ac = CollisionsSystem::Get().IsColliding(*ccc, *cccb, *e, *eb);
#else
	const auto ac = CollisionsSystem::Get().IsColliding(*cccb, *ccc, *eb, *e);
#endif
	
	//std::cout << ac;
}


void MainScene::OnUpdate() {
	Scene::OnUpdate();
}

void MainScene::OnDraw() {

}

void MainScene::OnEvent() {
	
}