#pragma once
#include "Systems/ECSystem.h"
#include "Entities/Entity.h"

//##############################################################################
//##------------------------- FORWARD DECLARATIONS ---------------------------##
//##############################################################################

class GameManager;

//##############################################################################
//##----------------------------- ENUMERATIONS -------------------------------##
//##############################################################################

template<typename S>
concept DerivedFromSystem = std::is_base_of_v<ECSystem, S>;
template<typename C>
concept DerivedFromComponent = std::is_base_of_v<Component, C>;

class SystemsManager {

//##############################################################################
//##------------------------------ ATTRIBUTES --------------------------------##
//##############################################################################
	
private:
	/* FLAGS */
	bool m_Initialized;

private:
    std::array<ECSystem*, static_cast<size_t>(SystemType::MaxComponents)> m_Systems;
	GameManager* m_GameManager;
	Entity** m_Entities; //Ptr to the array of entities (GameManager)
	std::list<Component*>** m_Components; //Array of list of components

//##############################################################################
//##--------------------------------- CLASS ----------------------------------##
//##############################################################################
	
public:
	SystemsManager();
	~SystemsManager();

	bool Init();
    void UnInit();

	static SystemsManager& Get();

	template<DerivedFromSystem S>
	void RegisterSystem();

	//Systems
	void Start(Entity** entities);
	void Start(Entity& entity);
	void Update();
	
	//--- COMPONENTS ---//
	template<DerivedFromComponent C>
	_NODISCARD C* GetComponentOfType(const Entity& entity) const;

	template<DerivedFromComponent C>
	C* AddComponent(Entity& entity);

	template<DerivedFromComponent C>
	bool RemoveComponent(Entity& entity);

	template<DerivedFromComponent C>
	_NODISCARD bool HasComponent(const Entity& entity) const;

	GETTER bool HasAnyComponent(const Entity& entity) const { return entity.m_HasComponentsMask != 0; }
	
	void RemoveAllComponents(const Entity& entity);

	SETTER _NODISCARD void* ResizeComponentsArray(const int newSize) { return realloc(m_Components, newSize); }

	void SwapComponentsList(int indexFrom, int indexTo);
	//----------------//
	
};

#include "SystemsManager.inl"