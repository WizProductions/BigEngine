
#include "pch.h"
#include "SystemsManager.h"
#include "Managers/GameManager.h"
#include "Systems/HealthSystem.h"
#include "Systems/ScriptsSystem.h"
#include "Components/Component.h"
#include "Systems/CollisionsSystem.h"

SystemsManager::SystemsManager() { this->UnInit(); }
SystemsManager::~SystemsManager() { DESTRUCTOR_UNINIT(m_Initialized); }

bool SystemsManager::Init() {
	
	if (m_Initialized)
		return false;

	//Register all systems
	RegisterSystem<ScriptsSystem>();
	RegisterSystem<HealthSystem>();
	RegisterSystem<CollisionsSystem>();

	INIT_PTR(m_GameManager, GameManager::Get());
	INIT_PTR(m_Entities, m_GameManager->GetAllEntities());
	INIT_NEW_PTR(m_Components, std::list<Component*>*[m_GameManager->entityArraySize](nullptr));
	
	m_Initialized = true;

	return true;
}

void SystemsManager::UnInit() {

	if (m_Initialized) {
		for (const auto& system : m_Systems) {
			if (system != nullptr)
				system->UnInit();
		}
	}

	m_GameManager = nullptr;
	m_Systems.fill(nullptr);
	m_Entities = nullptr;

	if (m_Components && *m_Components) {
		const std::list<Component*>* componentList = *m_Components;
		for (const Component* component : *componentList) {
			delete component;
		}
		delete componentList;
		delete m_Components;
		m_Components = nullptr;
	}

	
	m_Initialized = false;
}

SystemsManager& SystemsManager::Get() {
	static SystemsManager mInstance;
	return mInstance;
}

void SystemsManager::Start(Entity** entities) {

	for (const auto system : m_Systems) {
		if (system != nullptr)
			system->Start(entities, m_Components);
	}
}

void SystemsManager::Start(Entity& entity) {
	
	for (const auto system : m_Systems) {
		if (system != nullptr)
			system->Start(entity);
	}	
}

void SystemsManager::Update() {
	
	for (const auto system : m_Systems) {
		if (system != nullptr)
			system->Update();
	}
}

void SystemsManager::RemoveAllComponents(const Entity& entity) {
	//TODO: PUSHBACK TO DELETE LATER?
	
	for (const auto component : *m_Components[entity.m_SharedIndex]) {
		delete component;
	}
	
	delete m_Components[entity.m_SharedIndex];
	m_Components[entity.m_SharedIndex] = nullptr;
}

void SystemsManager::SwapComponentsList(const int indexFrom, const int indexTo) {

	const auto entityTo = m_Components[indexTo];
	m_Components[indexTo] = m_Components[indexFrom];
	m_Components[indexFrom] = entityTo;
}
