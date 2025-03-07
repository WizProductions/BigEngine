#include "pch.h"
#include "HealthSystem.h"

HealthSystem::HealthSystem() { this->UnInit(); }
HealthSystem::~HealthSystem() { DESTRUCTOR_UNINIT(m_Initialized); }

bool HealthSystem::Init() {

	ECSystem::Init();

	if (m_Initialized) {
		return false;
	}

	m_SystemType = SystemType::Health;
	m_SystemID = SystemId::Health;

	m_Initialized = true;

	return true;
}

void HealthSystem::UnInit() {

	if (!m_Initialized) {
		return;
	}

	m_Initialized = false;
}

void HealthSystem::Update() {}

HealthSystem& HealthSystem::Get() {
	static HealthSystem mInstance;
	return mInstance;
}

float HealthSystem::GetHealthPoints(const Entity& entity) const {

	const HealthComponent* healthComponent = SystemsManager::Get().GetComponentOfType<HealthComponent>(entity);
	
	if (healthComponent == nullptr) {
		return -1.f;
	}

	return healthComponent->m_Health;
}

void HealthSystem::SetHealthPoints(const Entity& entity, const float healthPoints) {
	
	HealthComponent* healthComponent = SystemsManager::Get().GetComponentOfType<HealthComponent>(entity);

	if (healthComponent == nullptr) {
		return;
	}

	healthComponent->m_Health = healthPoints;
}

bool HealthSystem::UpdateHealthPoints(const Entity& entity, const float delta) {
	
	HealthComponent* healthComponent = SystemsManager::Get().GetComponentOfType<HealthComponent>(entity);
	
	if (healthComponent == nullptr) {
		return false;
	}

	healthComponent->m_Health += delta;

	if (healthComponent->m_Health > healthComponent->m_MaxHealth) {
		healthComponent->m_Health = healthComponent->m_MaxHealth;
	}

	if (healthComponent->m_Health <= 0.f) {
		healthComponent->m_Health = 0.f;
		healthComponent->isDead = true;
	}

	return true;
}
