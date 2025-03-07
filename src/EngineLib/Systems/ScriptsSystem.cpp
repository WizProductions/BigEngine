#include "pch.h"
#include "ScriptsSystem.h"

ScriptsSystem::ScriptsSystem() { this->UnInit(); }
ScriptsSystem::~ScriptsSystem() { DESTRUCTOR_UNINIT(m_Initialized); }

bool ScriptsSystem::Init() {

	ECSystem::Init();
	
	if (m_Initialized)
		return false;

	m_SystemType = SystemType::Scripts;
	m_SystemID = SystemId::Scripts;
	m_SceneScript = nullptr;

	m_Initialized = true;

	return true;
}

void ScriptsSystem::UnInit() {

	m_SceneScript = nullptr;
	m_Initialized = false;
}

void ScriptsSystem::Start(Entity& entity) {

	if (entity.m_AttachedScript == nullptr)
		return;

	entity.m_AttachedScript->Start();
}

void ScriptsSystem::Start(Entity** entities, std::list<Component*>** components) {
	ECSystem::Start(entities, components);

	m_SceneScript = &GameManager::Get()->GetScene()->GetSceneScript();

	m_SceneScript->Start();
}

void ScriptsSystem::Update() {
	
	m_SceneScript->Update();

	for (int i(0); i <= GameManager::lastEntityIndex; i++) {

		if (m_Entities[i]->m_AttachedScript == nullptr)
			continue;
		
		m_Entities[i]->m_AttachedScript->Update();
	}
}

ScriptsSystem& ScriptsSystem::Get() {
	static ScriptsSystem mInstance;
	return mInstance;
}
