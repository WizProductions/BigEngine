
#include "pch.h"
#include <Scenes/Scene.h>

Scene::Scene() { Scene::UnInit(); }

Scene::~Scene() { DESTRUCTOR_UNINIT(m_Initialized); }

void Scene::Init(GameManager* GameManager) {

	if (m_Initialized)
		return;

	m_SceneFinished = false;
	m_SceneScript = nullptr;
	
	m_MyGameManager = GameManager;
	m_Initialized = true;
}

void Scene::UnInit() {
	
	m_MyGameManager = nullptr;
	m_SceneFinished = false;
	m_SceneScript = nullptr;
	m_Initialized = false;
}

void Scene::OnSceneEnded() { this->UnInit(); }

void Scene::OnUpdate() {
	
	if (m_SceneFinished)
		OnSceneEnded();
}