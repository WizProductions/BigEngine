#pragma once

using namespace DirectX;

struct Transform {

	//##############################################################################
	//##------------------------------- STRUCTURES -------------------------------##
	//##############################################################################

		//Position
	XMFLOAT3 vPosition;
	XMFLOAT4X4 mPosition;

	//Rotation
	XMFLOAT3 vForward;
	XMFLOAT3 vRight;
	XMFLOAT3 vUp;
	XMFLOAT3 vOrigin;
	XMFLOAT4 qRotation;
	XMFLOAT3 vCachedLocalRotation;
	XMFLOAT3 vCachedWorldRotation;
	XMFLOAT4X4 mRotation;

	//Scale
	XMFLOAT3 vScale;
	XMFLOAT4X4 mScale;

	//Transform matrix, shader accept only matrix format.
	XMFLOAT4X4 mWorld;
	bool bTransformMatrixDirty = false; // If another one is set to true -> this is true, not needed

	//##############################################################################
	//##------------------------------- ATTRIBUTS --------------------------------##
	//##############################################################################

private:

	//#############################################################################
	//##--------------------------------- CLASS ---------------------------------##
	//#############################################################################


public:

	/*----------< CONSTRUCTORS >----------*/

	/** Constructor */
	Transform() { this->Identity(); }
	~Transform() = default;

	/*------------------------------------*/

	void Identity();
	void Identity(XMFLOAT4X4& matrix);

	//===== Position =====//
	_NODISCARD const XMFLOAT3& GetPosition() const { return vPosition; }
	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& position);

	//===== Rotation =====//
	_NODISCARD XMVECTOR XM_CALLCONV GetLocalRotation() { return XMLoadFloat3(&vCachedLocalRotation); }
	_NODISCARD XMVECTOR XM_CALLCONV GetWorldRotation() { return XMLoadFloat3(&vCachedWorldRotation); }
	_NODISCARD XMVECTOR XM_CALLCONV GetForwardVector() { return XMLoadFloat3(&vForward); };
	_NODISCARD XMVECTOR XM_CALLCONV GetBackwardVector() { return -XMLoadFloat3(&vForward); };
	_NODISCARD XMVECTOR XM_CALLCONV GetRightVector() { return XMLoadFloat3(&vRight); };
	_NODISCARD XMVECTOR XM_CALLCONV GetLeftVector() { return -XMLoadFloat3(&vRight); };
	_NODISCARD XMVECTOR XM_CALLCONV GetUpVector() { return XMLoadFloat3(&vUp); };
	_NODISCARD XMVECTOR XM_CALLCONV GetDownVector() { return -XMLoadFloat3(&vUp); };
	_NODISCARD XMVECTOR XM_CALLCONV GetRotationQuaternion() { return XMLoadFloat4(&qRotation); };

	/** Returns true if the transform have a rotation (by checking qRotation.w) */
	GETTER bool IsRotated() { return (qRotation.w != 1.f); }

	//===== World Rotation =====//
	/** @param pitch: degrees value */
	/** @param yaw: degrees value */
	/** @param roll: degrees value */
	void SetWorldRotation(float pitch, float yaw, float roll);
	/** @param pitch: degrees value */
	/** @param yaw: degrees value */
	/** @param roll: degrees value */
	void WorldRotate(float pitch, float yaw, float roll);
	/** @param angle: degrees value */
	void RotateWorldPitch(float angle);
	/** @param angle: degrees value */
	void RotateWorldYaw(float angle);
	/** @param angle: degrees value */
	void RotateWorldRoll(float angle);

	//===== Local Rotation =====//
	/** @param pitch: degrees value */
	/** @param yaw: degrees value */
	/** @param roll: degrees value */
	void SetLocalRotation(float pitch, float yaw, float roll);
	/** @param pitch: degrees value */
	/** @param yaw: degrees value */
	/** @param roll: degrees value */
	void LocalRotate(float pitch, float yaw, float roll);
	/** @param angle: degrees value */
	void LocalRotatePitch(float angle);
	/** @param angle: degrees value */
	void LocalRotateYaw(float angle);
	/** @param angle: degrees value */
	void LocalRotateRoll(float angle);

	/** Format the angle to use the clockwise rotation system. */
	/** @param angle: degrees value */
	_NODISCARD float ToClockWiseRotationFormat(float angle);

	/** Remove float-precision errors */
	void CleanRotationVectors();

	//===== Scale =====//
	_NODISCARD XMVECTOR XM_CALLCONV GetScale() { return XMLoadFloat3(&vScale); };
	void SetScale(float x, float y, float z);
	void SetScale(const XMFLOAT3& scale);
	void SetScale(const XMVECTOR& scale);

	std::string Print(
		bool position = true,
		bool rotation = false,
		bool scale = false,
		bool transform = false,
		bool matrix = false,
		bool quaternion = false
	) const;

	static XMFLOAT3 ExtractPitchYawRollFromMatrix(const XMMATRIX& matrix);

private:
	void UpdatePositionMatrix();
	void UpdateRotationMatrix();
	void UpdateScaleMatrix();

public:
	void UpdateTransformMatrix();
};

std::ostream& operator<<(std::ostream& os, const Transform& transform);