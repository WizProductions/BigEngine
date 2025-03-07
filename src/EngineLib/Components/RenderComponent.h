#pragma once
#include "Component.h"

struct RenderComponent final : Component {

//##############################################################################
//##------------------------------- ATTRIBUTES --------------------------------##
//##############################################################################

public:
	static inline SystemType ComponentType = SystemType::Render;

	wchar_t ShaderLink = *L"..\\..\\..\\src\\EngineDev\\Shaders\\color.hlsl";
	//Texture
	//

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################

/*----------< CONSTRUCTORS >----------*/

	RenderComponent() = default;
	~RenderComponent() override = default;

/*------------------------------------*/

	GETTER virtual SystemType GetComponentType() override { return ComponentType; }
	
};