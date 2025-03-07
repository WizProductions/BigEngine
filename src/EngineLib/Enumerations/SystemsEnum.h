#pragma once

//##############################################################################
//##------------------------------ ENUMERATIONS ------------------------------##
//##############################################################################

enum class SystemType : uint32_t {
	Scripts = 1,
	Physics = 2, 
	Health = 4,
	Camera = 8,
	Collisions = 16,
	Render = 32,

	MaxComponents = 5
};

enum class SystemId : u_char {
	Scripts = 0,
	Physics = 1,
	Health = 2,
	Camera = 3,
	Collisions = 4,
	Render = 5,

	MaxComponents = 6
};