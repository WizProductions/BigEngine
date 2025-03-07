#pragma once

#include "GameManager.h"

template <typename S, typename Ss>
void ApplicationManager::StartGame() const {
	
	GameManager* gameManager = GameManager::Get();
	gameManager->Init();
	
	gameManager->StartScene<S, Ss>();
	gameManager->StartLoop();
}