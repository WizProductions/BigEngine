#pragma once
#include "Components/AABBCollisionComponent.h"
#include "Components/OBBCollisionComponent.h"
#include "Components/SphereCollisionComponent.h"
#include "Entities/Entity.h"

#undef min
#undef max

//INFORMATIONS POUR LE RAPPORT :
//Dans le contexte des collisions, nous avons une optimisation de fait, nous testons OBB x OBB mais si ceux-ci ne sont pas tournés, ce qui correspond à un cas de AABB finalement,
//donc nous les testons en AABB x AABB.

//INCOHERENCE POTENTIELS :
//je ne suis pas sur des collisions :
//- Sphere x OBB
//- AABB x OBB
//- OBB x OBB
//PROBLEMES :
//Les templates c'est de la merde très clairement, si const auto ac = CollisionsSystem::Get().IsColliding(*typeAABB, *typeSphere, *e, *eb); est appelé, le compilateur pète un cable car
// il essaie de faire les static cast qui sont dans les if à cause de ces templates de merde j'imagine, si j'inverse les deux types cela fonctionne, les OBB ne pose pas de problèlme sauf
//Si j'en instancie un, bref ce code pue la merde mais je sais pas faire autrement car on a rien appris sur ça et comment faire un bon ECS ^^

template <typename C1, typename C2>
CollidingResult CollisionsSystem::IsColliding(const C1& cA, const C2& cB, Entity& entityA, Entity& entityB) {

    if (cA.m_CollisionType == AABB && cB.m_CollisionType == AABB) { // AABB x AABB
        return IsColliding<AABBCollisionComponent, AABBCollisionComponent>(
            static_cast<const AABBCollisionComponent&>(cA),
            static_cast<const AABBCollisionComponent&>(cB),
            entityA,
            entityB
        );
    }
    if ((cA.m_CollisionType == SPHERE && cB.m_CollisionType == AABB) ||
        (cA.m_CollisionType == AABB && cB.m_CollisionType == SPHERE)) {  // Sphere x AABB ou AABB x Sphere
       
        if (cA.m_CollisionType == SPHERE) {
            return IsColliding<SphereCollisionComponent, AABBCollisionComponent>(
                static_cast<const SphereCollisionComponent&>(cA),
                static_cast<const AABBCollisionComponent&>(cB),
                entityA,
                entityB
            );
        }

        m_bRevertedCollisionTest = true;
        return IsColliding<SphereCollisionComponent, AABBCollisionComponent>(
            static_cast<const SphereCollisionComponent&>(cB),
            static_cast<const AABBCollisionComponent&>(cA),
            entityB,
            entityA
        );
    }
    if (cA.m_CollisionType == SPHERE && cB.m_CollisionType == SPHERE) { // Sphere x Sphere
        return IsColliding<SphereCollisionComponent, SphereCollisionComponent>(
            static_cast<const SphereCollisionComponent&>(cA),
            static_cast<const SphereCollisionComponent&>(cB),
            entityA,
            entityB
        );
    }
    if (cA.m_CollisionType == OBB && cB.m_CollisionType == OBB) { // OBB x OBB
        return IsColliding<OBBCollisionComponent, OBBCollisionComponent>(
            static_cast<const OBBCollisionComponent&>(cA),
            static_cast<const OBBCollisionComponent&>(cB),
            entityA,
            entityB
        );
    }
    if ((cA.m_CollisionType == SPHERE && cB.m_CollisionType == OBB) ||
        (cA.m_CollisionType == OBB && cB.m_CollisionType == SPHERE)) { // Sphere x OBB ou OBB x Sphere
        
        if (cA.m_CollisionType == SPHERE) {
            return IsColliding<SphereCollisionComponent, OBBCollisionComponent>(
                static_cast<const SphereCollisionComponent&>(cA),
                static_cast<const OBBCollisionComponent&>(cB),
                entityA,
                entityB
            );
        }
        
        m_bRevertedCollisionTest = true;
        return IsColliding<SphereCollisionComponent, OBBCollisionComponent>(
            static_cast<const SphereCollisionComponent&>(cB),
            static_cast<const OBBCollisionComponent&>(cA),
            entityB,
            entityA
        );
    }
    if ((cA.m_CollisionType == AABB && cB.m_CollisionType == OBB) ||
        (cA.m_CollisionType == OBB && cB.m_CollisionType == AABB)) { // AABB x OBB ou OBB x AABB
       
        if (cA.m_CollisionType == AABB) {
            return IsColliding<AABBCollisionComponent, OBBCollisionComponent>(
                static_cast<const AABBCollisionComponent&>(cA),
                static_cast<const OBBCollisionComponent&>(cB),
                entityA,
                entityB
            );
        }
        
        m_bRevertedCollisionTest = true;
        return IsColliding<AABBCollisionComponent, OBBCollisionComponent>(
            static_cast<const AABBCollisionComponent&>(cB),
            static_cast<const OBBCollisionComponent&>(cA),
            entityB,
            entityA
        );
    }
    
    ASSERT_MSG(false, "CollisionComponent invalid, Fix is required!");
    return {};
}


//AABB x AABB
template <>
inline CollidingResult CollisionsSystem::IsColliding<AABBCollisionComponent, AABBCollisionComponent>(
	const AABBCollisionComponent& cA, const AABBCollisionComponent& cB, Entity& entityA, Entity& entityB) {

	//CENTERS
	const XMFLOAT3& entity_A_Center = entityA.m_transform.GetPosition();
	const XMFLOAT3& entity_B_Center = entityB.m_transform.GetPosition();

	//RECT SIZES
	const XMFLOAT3& entity_A_RectSize = cA.m_RectSize;
	const XMFLOAT3& entity_B_RectSize = cB.m_RectSize;

	//Entity A
	const float A_MinX = entity_A_Center.x - entity_A_RectSize.x * 0.5f;
	const float A_MinY = entity_A_Center.y - entity_A_RectSize.x * 0.5f;
	const float A_MinZ = entity_A_Center.z - entity_A_RectSize.x * 0.5f;
	const float A_MaxX = A_MinX + entity_A_RectSize.x;
	const float A_MaxY = A_MinY + entity_A_RectSize.y;
	const float A_MaxZ = A_MinZ + entity_A_RectSize.z;

	//Entity B
	const float B_MinX = entity_B_Center.x - entity_B_RectSize.x * 0.5f;
	const float B_MinY = entity_B_Center.y - entity_B_RectSize.x * 0.5f;
	const float B_MinZ = entity_B_Center.z - entity_B_RectSize.x * 0.5f;
	const float B_MaxX = B_MinX + entity_B_RectSize.x;
	const float B_MaxY = B_MinY + entity_B_RectSize.y;
	const float B_MaxZ = B_MinZ + entity_B_RectSize.z;

	
	if (!(A_MinX < B_MaxX) || !(A_MaxX > B_MinX)) //X
		return {}; //No collision X

	if (!(A_MinY < B_MaxY) || !(A_MaxY > B_MinY)) //Y
		return {}; //No collision Y

	if (!(A_MinZ < B_MaxZ) || !(A_MaxZ > B_MinZ)) //Z
		return {}; //No collision Z


	//Collision
	
	const float overlapX = std::min(A_MaxX, B_MaxX) - std::max(A_MinX, B_MinX);
	const float overlapY = std::min(A_MaxY, B_MaxY) - std::max(A_MinY, B_MinY);
	const float overlapZ = std::min(A_MaxZ, B_MaxZ) - std::max(A_MinZ, B_MinZ);
	
	float penetrationDepth = overlapX;
	XMFLOAT3 collisionNormal = { (entity_B_Center.x > entity_A_Center.x) ? -1.0f : 1.0f, 0.0f, 0.0f };

	if (overlapY < penetrationDepth)
	{
		penetrationDepth = overlapY;
		collisionNormal = { 0.0f, (entity_B_Center.y > entity_A_Center.y) ? -1.0f : 1.0f, 0.0f };
	}
	if (overlapZ < penetrationDepth)
	{
		penetrationDepth = overlapZ;
		collisionNormal = { 0.0f, 0.0f, (entity_B_Center.z > entity_A_Center.z) ? -1.0f : 1.0f };
	}
	
	return {.result= true, .normal= collisionNormal, .entityCollided= &entityB, .penetrationDepth= penetrationDepth};
}

//Sphere x Sphere
template <>
inline CollidingResult CollisionsSystem::IsColliding<SphereCollisionComponent, SphereCollisionComponent>(
	const SphereCollisionComponent& cA, const SphereCollisionComponent& cB, Entity& entityA, Entity& entityB) {

	//CENTERS
	const XMVECTOR& entity_A_Center = XMLoadFloat3(&entityA.m_transform.GetPosition());
	const XMVECTOR& entity_B_Center = XMLoadFloat3(&entityB.m_transform.GetPosition());

	//RADIUS
	const float& entity_A_Radius = cA.m_Radius;
	const float& entity_B_Radius = cB.m_Radius;

	const XMVECTOR centerDelta = entity_B_Center - entity_A_Center;
	
	const double squaredDistance = XMVectorGetX(XMVector3LengthSq(centerDelta));
	const float radiusSum = entity_A_Radius + entity_B_Radius;
	
	if (squaredDistance > (radiusSum * radiusSum)) { //No collision
		return {};
	}
	
	//Collision
	XMFLOAT3 collisionNormal = {0.f, 0.f, 0.f};
	float approxDist = 0.0f;
	
	if (squaredDistance > 0.0001f) { // Not null
		const XMVECTOR invSqrt = XMVector3ReciprocalLength(centerDelta); //Reciprocal sqrt (fewer cycles needed)
		XMStoreFloat3(&collisionNormal, -centerDelta * invSqrt);
		
		approxDist = 1.0f / XMVectorGetX(invSqrt);
	}
	else {
		collisionNormal = { 1.0f, 1.f, 1.0f }; //Default normal, two entities are at the same position
		//TODO: RANDOM FOR DEFAULT NORMAL?
	}

	const float penetrationDepth = radiusSum - approxDist;
	
	return {.result= true, .normal= collisionNormal, .entityCollided= &entityB, .penetrationDepth= penetrationDepth};
}

//OBB x OBB
template <>
inline CollidingResult CollisionsSystem::IsColliding<OBBCollisionComponent, OBBCollisionComponent>(
    const OBBCollisionComponent& cA, const OBBCollisionComponent& cB, Entity& entityA, Entity& entityB) {

	//OPTIMIZATION TRY AABB IF ROTATION = 0°
	if (!entityA.m_transform.IsRotated() && !entityB.m_transform.IsRotated()) {
		return IsColliding<AABBCollisionComponent, AABBCollisionComponent>(static_cast<const AABBCollisionComponent>(cA), static_cast<const AABBCollisionComponent>(cB), entityA, entityB);
	}
	
    constexpr float epsilon = 0.0001f;

    //CENTERS
    const XMFLOAT3& entity_A_Center = entityA.m_transform.GetPosition();
    const XMFLOAT3& entity_B_Center = entityB.m_transform.GetPosition();

    //VECTORS NORMALIZED
    const XMVECTOR entity_A_ForwardVector = entityA.m_transform.GetForwardVector();
    const XMVECTOR entity_A_RightVector = entityA.m_transform.GetRightVector();
    const XMVECTOR entity_A_UpVector = entityA.m_transform.GetUpVector();

    const XMVECTOR entity_B_ForwardVector = entityB.m_transform.GetForwardVector();
    const XMVECTOR entity_B_RightVector = entityB.m_transform.GetRightVector();
    const XMVECTOR entity_B_UpVector = entityB.m_transform.GetUpVector();

    //RECT SIZES
    const XMFLOAT3& entity_A_RectSize = cA.m_RectSize;
    const XMFLOAT3& entity_B_RectSize = cB.m_RectSize;

    //HALF SIZES
    const float A_HalfX = entity_A_RectSize.x * 0.5f;
    const float A_HalfY = entity_A_RectSize.y * 0.5f;
    const float A_HalfZ = entity_A_RectSize.z * 0.5f;

    const float B_HalfX = entity_B_RectSize.x * 0.5f;
    const float B_HalfY = entity_B_RectSize.y * 0.5f;
    const float B_HalfZ = entity_B_RectSize.z * 0.5f;

    // AXES TO TEST
    std::vector<XMVECTOR> axesToTest = {
        entity_A_ForwardVector,
        entity_A_RightVector,
        entity_A_UpVector,
        entity_B_ForwardVector,
        entity_B_RightVector,
        entity_B_UpVector
    };

    // CROSS PRODUCTS
    XMVECTOR crossProducts[] = {
        XMVector3Cross(entity_A_ForwardVector, entity_B_ForwardVector),
        XMVector3Cross(entity_A_ForwardVector, entity_B_RightVector),
        XMVector3Cross(entity_A_ForwardVector, entity_B_UpVector),
        XMVector3Cross(entity_A_RightVector, entity_B_ForwardVector),
        XMVector3Cross(entity_A_RightVector, entity_B_RightVector),
        XMVector3Cross(entity_A_RightVector, entity_B_UpVector),
        XMVector3Cross(entity_A_UpVector, entity_B_ForwardVector),
        XMVector3Cross(entity_A_UpVector, entity_B_RightVector),
        XMVector3Cross(entity_A_UpVector, entity_B_UpVector)
    };

	//REMOVE PARALLEL AXES
    for (const XMVECTOR& cross : crossProducts) {
        if (XMVectorGetX(XMVector3LengthSq(cross)) > epsilon) {
            axesToTest.push_back(cross);
        }
    }
	
	float minPenetration = FLT_MAX;
	XMVECTOR bestAxis = XMVectorZero();
	float penetrationDepth = 0.f;
	
	const XMVECTOR centerToCenterVect = XMLoadFloat3(&entity_B_Center) - XMLoadFloat3(&entity_A_Center);
	
    //CHECK COLLISION
	for (const XMVECTOR& axis : axesToTest) {

		XMVECTOR normAxis = XMVector3Normalize(axis);

		const float entity_A_ProjectedAxe =
			A_HalfX * fabs(XMVectorGetX(XMVector3Dot(entity_A_RightVector, normAxis))) +
			A_HalfY * fabs(XMVectorGetX(XMVector3Dot(entity_A_UpVector, normAxis))) +
			A_HalfZ * fabs(XMVectorGetX(XMVector3Dot(entity_A_ForwardVector, normAxis)));

		const float entity_B_ProjectedAxe =
			B_HalfX * fabs(XMVectorGetX(XMVector3Dot(entity_B_RightVector, normAxis))) +
			B_HalfY * fabs(XMVectorGetX(XMVector3Dot(entity_B_UpVector, normAxis))) +
			B_HalfZ * fabs(XMVectorGetX(XMVector3Dot(entity_B_ForwardVector, normAxis)));

		const float centerDistance = fabs(XMVectorGetX(XMVector3Dot(normAxis, centerToCenterVect)));

		if (centerDistance > entity_A_ProjectedAxe + entity_B_ProjectedAxe) {
			return {}; //NO COLLISION
		}

		float penetrationDepth = (entity_A_ProjectedAxe + entity_B_ProjectedAxe) - centerDistance;
		if (penetrationDepth < minPenetration) {
			minPenetration = penetrationDepth;

			const float dotProduct = XMVectorGetX(XMVector3Dot(normAxis, centerToCenterVect));
			bestAxis = dotProduct < 0 ? XMVectorNegate(normAxis) : normAxis;
		}
	}
	XMFLOAT3 collisionNormal = { 0.f, 0.f, 0.f };

	//COLLISION DETECTED

	//STORE THE NORMAL TO RETURN
	XMStoreFloat3(&collisionNormal, XMVector3Normalize(bestAxis));
	
    return {.result = true, .normal = collisionNormal, .entityCollided = &entityB, .penetrationDepth = minPenetration};
}

// Sphere x AABB
template <>
inline CollidingResult CollisionsSystem::IsColliding<SphereCollisionComponent, AABBCollisionComponent>(
    const SphereCollisionComponent& cA, const AABBCollisionComponent& cB, Entity& entityA, Entity& entityB)
{

	// POSITIONS
    const XMFLOAT3& sphereCenter = entityA.m_transform.GetPosition();
    const XMFLOAT3& boxCenter = entityB.m_transform.GetPosition();

    const float sphereRadius = cA.m_Radius;
    const XMFLOAT3& boxSize = cB.m_RectSize;
    
    // HALF SIZES
    const float boxHalfWidth = boxSize.x * 0.5f;
    const float boxHalfHeight = boxSize.y * 0.5f;
    const float boxHalfDepth = boxSize.z * 0.5f;
    
    const float boxMinX = boxCenter.x - boxHalfWidth;
    const float boxMinY = boxCenter.y - boxHalfHeight;
    const float boxMinZ = boxCenter.z - boxHalfDepth;
    const float boxMaxX = boxCenter.x + boxHalfWidth;
    const float boxMaxY = boxCenter.y + boxHalfHeight;
    const float boxMaxZ = boxCenter.z + boxHalfDepth;
	
    XMFLOAT3 closestPoint;
    closestPoint.x = std::max(boxMinX, std::min(sphereCenter.x, boxMaxX));
    closestPoint.y = std::max(boxMinY, std::min(sphereCenter.y, boxMaxY));
    closestPoint.z = std::max(boxMinZ, std::min(sphereCenter.z, boxMaxZ));
	
    const XMVECTOR vClosestPoint = XMLoadFloat3(&closestPoint);
    const XMVECTOR vSphereCenter = XMLoadFloat3(&sphereCenter);
    XMVECTOR vDelta = vClosestPoint - vSphereCenter;
	
    const float distSq = XMVectorGetX(XMVector3LengthSq(vDelta));
	
    if (distSq > (sphereRadius * sphereRadius)) // NO COLLISION
    {
        return {};
    }

	//COLLISION
    float penetrationDepth = 0.0f;
    XMVECTOR collisionNormalVec = XMVectorZero();
    
    if (distSq > 0.0001f) {
    	
        XMVECTOR vInvLength = XMVector3ReciprocalLength(vDelta);
        collisionNormalVec = -vDelta * vInvLength;
    	
        float approxDist = 1.0f / XMVectorGetX(vInvLength);
        penetrationDepth = sphereRadius - approxDist;
    }
    else
    {
    	float dLeft = sphereCenter.x - boxMinX;
    	float dRight = boxMaxX - sphereCenter.x;
    	float dBottom = sphereCenter.y - boxMinY;
    	float dTop = boxMaxY - sphereCenter.y;
    	float dBack = sphereCenter.z - boxMinZ;
    	float dFront = boxMaxZ - sphereCenter.z;

    	float minDist = dLeft;
    	int minAxis = -1;
    	bool isPositive = false;

    	if (dLeft < minDist) {
    		minDist = dLeft;
    		minAxis = 0;
    		isPositive = false;
    	}
    	if (dRight < minDist) {
    		minDist = dRight;
    		minAxis = 0;
    		isPositive = true;
    	}
    	if (dBottom < minDist) {
    		minDist = dBottom;
    		minAxis = 1;
    		isPositive = false;
    	}
    	if (dTop < minDist) {
    		minDist = dTop;
    		minAxis = 1;
    		isPositive = true;
    	}
    	if (dBack < minDist) {
    		minDist = dBack;
    		minAxis = 2;
    		isPositive = false;
    	}
    	if (dFront < minDist) {
    		minDist = dFront;
    		minAxis = 2;
    		isPositive = true;
    	}
    
    	float normalX = 0.0f, normalY = 0.0f, normalZ = 0.0f;
    	
    	if (minAxis == 0) normalX = isPositive ? 1.0f : -1.0f;
    	else if (minAxis == 1) normalY = isPositive ? 1.0f : -1.0f;
    	else normalZ = isPositive ? 1.0f : -1.0f;
    
    	collisionNormalVec = XMVectorSet(normalX, normalY, normalZ, 0.0f);
    
    	penetrationDepth = minDist + sphereRadius; // Correction ici
    }
	
    XMFLOAT3 outNormal;
    XMStoreFloat3(&outNormal, collisionNormalVec);
	
    if (m_bRevertedCollisionTest)
    {
        m_bRevertedCollisionTest = false;
        outNormal.x = -outNormal.x;
        outNormal.y = -outNormal.y;
        outNormal.z = -outNormal.z;
        return { .result= true, .normal= outNormal, .entityCollided= &entityA, .penetrationDepth= penetrationDepth};
    }

    return { .result= true, .normal= outNormal, .entityCollided= &entityB, .penetrationDepth= penetrationDepth};
}

//Sphere x OBB
template <>
inline CollidingResult CollisionsSystem::IsColliding<SphereCollisionComponent, OBBCollisionComponent>(
    const SphereCollisionComponent& cA, const OBBCollisionComponent& cB, Entity& entityA, Entity& entityB) {

    // Optimisation : traiter comme AABB si pas de rotation
    if (!entityB.m_transform.IsRotated())
        return IsColliding<SphereCollisionComponent, AABBCollisionComponent>(
            cA, static_cast<const AABBCollisionComponent>(cB), entityA, entityB);

    // Centres
    const XMVECTOR entity_A_Center = XMLoadFloat3(&entityA.m_transform.GetPosition());
    const XMVECTOR entity_B_Center = XMLoadFloat3(&entityB.m_transform.GetPosition());

    // Axes normalisés de l'OBB
    const XMVECTOR entity_B_ForwardVector = entityB.m_transform.GetForwardVector();
    const XMVECTOR entity_B_RightVector = entityB.m_transform.GetRightVector();
    const XMVECTOR entity_B_UpVector = entityB.m_transform.GetUpVector();

    // Tailles
    const float entity_A_Radius = cA.m_Radius;
    const float entity_A_RadiusSq = entity_A_Radius * entity_A_Radius;
    const XMFLOAT3& entity_B_RectSize = cB.m_RectSize;

    // Vecteur de distance
    const XMVECTOR distVect = entity_A_Center - entity_B_Center;

    // Demi-tailles
    const float B_HalfX = entity_B_RectSize.x * 0.5f;
    const float B_HalfY = entity_B_RectSize.y * 0.5f;
    const float B_HalfZ = entity_B_RectSize.z * 0.5f;

    // Projections sur les axes de l'OBB
    float p1 = XMVectorGetX(XMVector3Dot(distVect, entity_B_RightVector));  // Projection sur X local de l'OBB
    float p2 = XMVectorGetX(XMVector3Dot(distVect, entity_B_UpVector));     // Projection sur Y local de l'OBB
    float p3 = XMVectorGetX(XMVector3Dot(distVect, entity_B_ForwardVector)); // Projection sur Z local de l'OBB

    // Clamp pour trouver le point le plus proche
    float p1_clamped = clamp(p1, -B_HalfX, B_HalfX);
    float p2_clamped = clamp(p2, -B_HalfY, B_HalfY);
    float p3_clamped = clamp(p3, -B_HalfZ, B_HalfZ);

    // Calcul du point le plus proche sur l'OBB
    const XMVECTOR nearest_B_Point = entity_B_Center +
        (p1_clamped * entity_B_RightVector) +
        (p2_clamped * entity_B_UpVector) +
        (p3_clamped * entity_B_ForwardVector);

    // Vecteur du centre de la sphère au point le plus proche sur l'OBB
    const XMVECTOR closestVector = entity_A_Center - nearest_B_Point;
    const float closestDistSq = XMVectorGetX(XMVector3Dot(closestVector, closestVector));

    // Pas de collision si la distance carrée est plus grande que le rayon carré
    if (closestDistSq > entity_A_RadiusSq) {
        return {};
    }

    // Collision détectée
    XMVECTOR collisionNormalVec;
    float penetrationDepth;

    // Si la distance est négligeable ou nulle, choisir un axe principal comme normale
    if (closestDistSq < 0.0001f) {
        // Trouver l'axe principal pour la normale (celui avec la plus petite pénétration)
        float abs_p1 = fabs(p1);
        float abs_p2 = fabs(p2);
        float abs_p3 = fabs(p3);
        
        float p1_penetration = B_HalfX - abs_p1 + entity_A_Radius;
        float p2_penetration = B_HalfY - abs_p2 + entity_A_Radius;
        float p3_penetration = B_HalfZ - abs_p3 + entity_A_Radius;
        
        if (p1_penetration <= p2_penetration && p1_penetration <= p3_penetration) {
            collisionNormalVec = p1 >= 0 ? entity_B_RightVector : XMVectorNegate(entity_B_RightVector);
            penetrationDepth = p1_penetration;
        } else if (p2_penetration <= p1_penetration && p2_penetration <= p3_penetration) {
            collisionNormalVec = p2 >= 0 ? entity_B_UpVector : XMVectorNegate(entity_B_UpVector);
            penetrationDepth = p2_penetration;
        } else {
            collisionNormalVec = p3 >= 0 ? entity_B_ForwardVector : XMVectorNegate(entity_B_ForwardVector);
            penetrationDepth = p3_penetration;
        }
    } else {
        // Normaliser le vecteur de distance pour obtenir la normale
        float closestDist = XMVectorGetX(XMVector3Length(closestVector));
        collisionNormalVec = XMVectorDivide(closestVector, XMVectorReplicate(closestDist));
        
        // Calculer la pénétration comme (rayon - distance)
        penetrationDepth = entity_A_Radius - closestDist;
    }

    // Stocker la normale et vérifier le cas inversé
    XMFLOAT3 outNormal;
    XMStoreFloat3(&outNormal, collisionNormalVec);

    if (m_bRevertedCollisionTest) {
        m_bRevertedCollisionTest = false;
        outNormal.x = -outNormal.x;
        outNormal.y = -outNormal.y;
        outNormal.z = -outNormal.z;
        return { .result = true, .normal = outNormal, .entityCollided = &entityA, .penetrationDepth = penetrationDepth };
    }

    return { .result = true, .normal = outNormal, .entityCollided = &entityB, .penetrationDepth = penetrationDepth };
}

//AABB x OBB
template <>
inline CollidingResult CollisionsSystem::IsColliding<AABBCollisionComponent, OBBCollisionComponent>(
    const AABBCollisionComponent& cA, const OBBCollisionComponent& cB, Entity& entityA, Entity& entityB) {

    //OPTIMIZATION TRY AABB IF ROTATION OF B = 0°
    if (!entityB.m_transform.IsRotated()) {
        if (m_bRevertedCollisionTest) {
            m_bRevertedCollisionTest = false;
            return IsColliding<AABBCollisionComponent, AABBCollisionComponent>(static_cast<const AABBCollisionComponent>(cB), cA, entityB, entityA);
        }
        return IsColliding<AABBCollisionComponent, AABBCollisionComponent>(cA, static_cast<const AABBCollisionComponent>(cB), entityA, entityB);
    }

    constexpr float epsilon = 0.0001f;

    //CENTERS
    const XMFLOAT3& entity_A_Center = entityA.m_transform.GetPosition();
    const XMFLOAT3& entity_B_Center = entityB.m_transform.GetPosition();

    //VECTORS NORMALIZED
    const XMVECTOR entity_B_ForwardVector = entityB.m_transform.GetForwardVector();
    const XMVECTOR entity_B_RightVector = entityB.m_transform.GetRightVector();
    const XMVECTOR entity_B_UpVector = entityB.m_transform.GetUpVector();

    //RECT SIZES
    const XMFLOAT3& entity_A_RectSize = cA.m_RectSize;
    const XMFLOAT3& entity_B_RectSize = cB.m_RectSize;

    //HALF SIZES
    const float A_HalfX = entity_A_RectSize.x * 0.5f;
    const float A_HalfY = entity_A_RectSize.y * 0.5f;
    const float A_HalfZ = entity_A_RectSize.z * 0.5f;

    const float B_HalfX = entity_B_RectSize.x * 0.5f;
    const float B_HalfY = entity_B_RectSize.y * 0.5f;
    const float B_HalfZ = entity_B_RectSize.z * 0.5f;


    const XMVECTOR centerToCenter = XMLoadFloat3(&entity_B_Center) - XMLoadFloat3(&entity_A_Center);
	
    std::array<XMVECTOR, 6> axesToTest = {
        XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), // AABB x-axis
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), // AABB y-axis
        XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), // AABB z-axis
        entity_B_RightVector, // OBB x-axis
        entity_B_UpVector, // OBB y-axis
        entity_B_ForwardVector // OBB z-axis
    };
    
    float minPenetration = FLT_MAX;
	
    for (int i = 0; i < axesToTest.size(); ++i) {
        XMVECTOR normalizedAxis = XMVector3Normalize(axesToTest[i]);
    	
        const float entity_A_ProjectedSize =
            A_HalfX * fabs(XMVectorGetX(XMVector3Dot(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), normalizedAxis))) +
            A_HalfY * fabs(XMVectorGetX(XMVector3Dot(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), normalizedAxis))) +
            A_HalfZ * fabs(XMVectorGetX(XMVector3Dot(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), normalizedAxis)));
    	
        const float entity_B_ProjectedSize =
            B_HalfX * fabs(XMVectorGetX(XMVector3Dot(entity_B_RightVector, normalizedAxis))) +
            B_HalfY * fabs(XMVectorGetX(XMVector3Dot(entity_B_UpVector, normalizedAxis))) +
            B_HalfZ * fabs(XMVectorGetX(XMVector3Dot(entity_B_ForwardVector, normalizedAxis)));
    	
        const float projectedDistance = fabs(XMVectorGetX(XMVector3Dot(centerToCenter, normalizedAxis)));
    	
        if (projectedDistance > entity_A_ProjectedSize + entity_B_ProjectedSize + epsilon) { //NO COLLISION
            return {};
        }

    	//COLLISION
    	
        const float penetration = entity_A_ProjectedSize + entity_B_ProjectedSize - projectedDistance;
        if (penetration < minPenetration) {
            minPenetration = penetration;
        }
    }

    XMVECTOR normalizedCenterToCenter = XMVector3Normalize(centerToCenter);
	
    float maxDot = -1.0f;
    XMVECTOR bestNormal = XMVectorZero();
    
    for (int i = 0; i < 3; ++i) {
        XMVECTOR axis = axesToTest[i];
        float dot = fabs(XMVectorGetX(XMVector3Dot(normalizedCenterToCenter, axis)));
        if (dot > maxDot) {
            maxDot = dot;
            float actualDot = XMVectorGetX(XMVector3Dot(normalizedCenterToCenter, axis));
            bestNormal = actualDot > 0 ? axis : XMVectorNegate(axis);
        }
    }
    
    // Stockage de la normale
    XMFLOAT3 outNormal;
    XMStoreFloat3(&outNormal, XMVectorNegate(bestNormal));
    
    if (m_bRevertedCollisionTest) {
        m_bRevertedCollisionTest = false;
        XMStoreFloat3(&outNormal, XMVectorNegate(bestNormal));
        return { .result = true, .normal = outNormal, .entityCollided = &entityA, .penetrationDepth = minPenetration };
    }
    
	return { .result = true, .normal = outNormal, .entityCollided = &entityB, .penetrationDepth = minPenetration };
}