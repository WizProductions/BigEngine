#include "pch.h"
#include "OBBCollisionComponent.h"

OBBCollisionComponent::operator AABBCollisionComponent() const {

	AABBCollisionComponent aabb;
	aabb.m_RectSize = this->m_RectSize;

	return aabb;
};	

OBBCollisionComponent::operator SphereCollisionComponent&() const {

	ASSERT_MSG(false, "OBB to Sphere Cast is not allowed in runtime.");
	SphereCollisionComponent sphere;
		
	return sphere;
};