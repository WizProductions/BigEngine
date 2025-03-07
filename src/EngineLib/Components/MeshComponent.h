#pragma once
#include "Component.h"

struct MeshComponent final : public Component {

	//##############################################################################
	//##-------------------------------- DATA ------------------------------------##
	//##############################################################################
	
	static inline SystemType ComponentType = SystemType::Health;

	//#############################################################################
	//##--------------------------------- CLASS ---------------------------------##
	//#############################################################################

	/*----------< CONSTRUCTORS >----------*/
	
	MeshComponent() = default;
	~MeshComponent() override = default;

	/*------------------------------------*/

	GETTER virtual SystemType GetComponentType() override { return ComponentType; }
	
};
