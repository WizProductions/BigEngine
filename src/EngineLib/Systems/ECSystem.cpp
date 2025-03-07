#include "pch.h"
#include "ECSystem.h"

ECSystem::ECSystem() { ECSystem::UnInit(); }
ECSystem::~ECSystem() { DESTRUCTOR_UNINIT(m_Initialized); }

bool ECSystem::Init() {

	if (m_Initialized)
		return false;
	
	m_Initialized = true;
	return true;
}

void ECSystem::UnInit() {

	m_Entities = nullptr;
	m_Components = nullptr;
	
	m_Initialized = false;
}

void ECSystem::Start(Entity& entity) {}
void ECSystem::Start(Entity** entities, std::list<Component*>** components) {
	m_Entities = entities;
	m_Components = components;
}

void ECSystem::OnComponentAdded(Entity& entity) {}