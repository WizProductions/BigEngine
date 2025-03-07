#include "pch.h"
#include "CollidingResult.h"

std::ostream& operator<<(std::ostream& os, const CollidingResult& collidingResult) {

#ifdef _DEBUG
	os << "==========> COLLIDING RESULT <===========" << std::endl;
	os << std::setprecision(2) << std::boolalpha << std::fixed;
	os << "Collided: " << collidingResult.result << std::endl; // Force true / false format
	os << "Collision Normal: " << collidingResult.normal << std::endl;
	os << "EntityCollided: " << collidingResult.entityCollided << " EntityIndex: " << IS_VALID(collidingResult.entityCollided, m_SharedIndex) << std::endl;
	os << "Collision penetration depth: " << collidingResult.penetrationDepth << std::endl;
	os << "=========================================" << std::endl;
#endif
	return os;
}