#include "pch.h"
#include "AABBCollisionComponent.h"

AABBCollisionComponent::operator OBBCollisionComponent() const {

	OBBCollisionComponent obb;
	obb.m_RectSize = this->m_RectSize;

	return obb;
};	

AABBCollisionComponent::operator SphereCollisionComponent&() const {

	ASSERT_MSG(false, "AABB to Sphere Cast is not allowed in runtime.");
	SphereCollisionComponent sphere;
		
	return sphere;
};