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
	Transform& m_transform;
	EntityScript* m_AttachedScript = nullptr;
	//Mesh m_Mesh;

	//#############################################################################
	//##--------------------------------- CLASS ---------------------------------##
	//#############################################################################


public:

	/*----------< CONSTRUCTORS >----------*/
	
	explicit Entity() :
		m_EntityID(GameManager::entityNextID++)
		, m_transform(*new Transform()) {
	}

	~Entity() {
		m_AttachedScript->End();
	}



	/*------------------------------------*/
	
};