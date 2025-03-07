#pragma once
#include "SystemsManager.h"

template<DerivedFromSystem S>
void SystemsManager::RegisterSystem() {
	
	S& system = S::Get();
	system.Init();
	m_Systems[static_cast<uint8_t>(system.GetSystemType())] = std::move(&system);
}

template <DerivedFromComponent C>
C* SystemsManager::GetComponentOfType(const Entity& entity) const {
 
	if (!HasComponent<C>(entity))
		return nullptr;
 
	for (auto component : *m_Components[entity.m_SharedIndex]) {
		if (C::ComponentType == component->GetComponentType()) {
			return reinterpret_cast<C*>(component);
		}
	}
 
	return nullptr;
}


template <DerivedFromComponent C>
C* SystemsManager::AddComponent(Entity& entity) {

	if (!this->HasAnyComponent(entity))
		m_Components[entity.m_SharedIndex] = new std::list<Component*>();
	
	else if (C* componentFound = this->GetComponentOfType<C>(entity))
		return componentFound;
	
	C* newComponent = new C();
	m_Components[entity.m_SharedIndex]->push_back(newComponent);
	entity.m_HasComponentsMask |= static_cast<uint32_t>(C::ComponentType);
	
	return newComponent;
}

template <DerivedFromComponent C>
bool SystemsManager::RemoveComponent(Entity& entity) {
	
	if (!this->HasComponent<C>(entity))
		return false;
	
	for (auto component : *m_Components[entity.m_SharedIndex]) {
		if (C::ComponentType == component->GetComponentType()) {
			
			delete component;
			m_Components[entity.m_SharedIndex]->remove(component);
			entity.m_HasComponentsMask &= ~static_cast<uint32_t>(C::ComponentType);
			
			return true;
		}
	}
	
	return false;
}

template<DerivedFromComponent C>
bool SystemsManager::HasComponent(const Entity& entity) const {
	return entity.m_HasComponentsMask & static_cast<uint32_t>(C::ComponentType);
}