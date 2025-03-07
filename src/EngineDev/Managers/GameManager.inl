#pragma once
#include "Scenes/Scene.h"
#include "Scripts/SceneScript.h"

template <typename S, typename Ss>
S* GameManager::StartScene() {
	static_assert(std::is_base_of_v<Scene, S>, "S must be derived from Scene");
	static_assert(std::is_base_of_v<SceneScript<S>, Ss>, "Ss must be derived from SceneScript");
    
	S* newScene = new S();
	Ss* sceneScript = new Ss();
	
	m_Scene = newScene;
	m_Scene->Init(this);
	m_Scene->AttachScriptToScene(sceneScript);

	sceneScript->m_AttachedScene = newScene;
	
	
	//TODO: Reset entities but still data of players + another scene system
	
	_ASSERT(newScene);
	_ASSERT(sceneScript);

	return newScene;
}