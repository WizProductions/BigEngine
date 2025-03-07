#pragma once

#ifdef IGNORE
#undef IGNORE
#endif

//##############################################################################
//##------------------------------ ENUMERATIONS ------------------------------##
//##############################################################################

enum CollisionType : char {
	SPHERE,
	AABB,
	OBB,

	UNKNOWN
};
	
enum CollisionMode : char {
	IGNORE,
	OVERLAP,
	BLOCK
};