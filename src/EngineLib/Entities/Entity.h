#pragma once

#include <Scripts/EntityScript.h>

struct Entity {

	//##############################################################################
	//##------------------------------- ATTRIBUTES --------------------------------##
	//##############################################################################

public:
	int m_EntityID = 0;
	int m_SharedIndex = 0;
	uint32_t m_HasComponentsMask = 0;
	Transform& m_Transform;
	EntityScript* m_AttachedScript = nullptr;
	//Geometry m_Geometry;  -> component

	//#############################################################################
	//##--------------------------------- CLASS ---------------------------------##
	//#############################################################################


public:

	/*----------< CONSTRUCTORS >----------*/
	
	explicit Entity() :
		m_EntityID(GameManager::entityNextID++)
		, m_Transform(*new Transform()) {
	}

	~Entity() {
		m_AttachedScript->End();
	}

	/*------------------------------------*/
	template<typename T>
	T* GetComponent() const {
		if (auto components = SystemsManager::Get().GetComponentsForEntity(*this)) {
			for (auto* component : *components) {
				if (auto* castedComponent = dynamic_cast<T*>(component)) {
					return castedComponent;
				}
			}
		}
		return nullptr;
	}

};