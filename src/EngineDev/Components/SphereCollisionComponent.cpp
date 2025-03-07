#include "pch.h"
#include "SphereCollisionComponent.h"

SphereCollisionComponent::operator AABBCollisionComponent&() const {

	ASSERT_MSG(false, "Sphere to AABB Cast is not allowed in runtime.");
	AABBCollisionComponent obb;

	return obb;
};	

SphereCollisionComponent::operator OBBCollisionComponent&() const {

	ASSERT_MSG(false, "Sphere to OBB Cast is not allowed in runtime.");
	OBBCollisionComponent aabb;;

	return aabb;
};