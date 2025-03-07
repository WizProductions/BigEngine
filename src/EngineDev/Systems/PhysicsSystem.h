#pragma once

#include "Systems/ECSystem.h"

class PhysicsSystem final : public ECSystem {

//#############################################################################
//##------------------------------ ATTRIBUTES -------------------------------##
//#############################################################################

private:
	bool m_Initialized;
	float m_FixedTimeStep;
	DirectX::XMFLOAT3 m_Gravity;
	DirectX::XMFLOAT3 m_Velocity;
	DirectX::XMFLOAT3 m_Acceleration;
	float m_Mass;
	bool m_UseGravity;

//#############################################################################
//##--------------------------------- CLASS ---------------------------------##
//#############################################################################

public:

/*----------< CONSTRUCTORS >----------*/

	PhysicsSystem();
	~PhysicsSystem() override;

	/*------------------------------------*/

	bool Init() override;
	void UnInit() override;
	void Update() override;

	/* GETTERS */

	static PhysicsSystem& Get();
	GETTER const DirectX::XMFLOAT3& GetVelocity() const { return m_Velocity; }
	GETTER const DirectX::XMFLOAT3& GetAcceleration() const { return m_Acceleration; }
	GETTER float GetMass() const { return m_Mass; }
	GETTER bool GetGravity() const { return m_UseGravity; }

	/* SETTERS */

	SETTER void SetGravity(const DirectX::XMFLOAT3& gravity) { m_Gravity = gravity; }
	SETTER void SetTimeStep(const float timeStep) { m_FixedTimeStep = timeStep; }
	SETTER void SetVelocity(const DirectX::XMFLOAT3& velocity) { m_Velocity = velocity; }
	SETTER void SetAcceleration(const DirectX::XMFLOAT3& acceleration) { m_Acceleration = acceleration; }
	SETTER void SetMass(const float mass) { m_Mass = mass; }

	/* OTHERS FUNCTIONS */ 
	void ApplyForce(const DirectX::XMFLOAT3& force);
};