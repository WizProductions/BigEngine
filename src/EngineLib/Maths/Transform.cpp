#include "pch.h"
#include "Transform.h"

/** Normalize in range [-360, 360] */
#define NormalizeDegreeAngle(angle) { \
	while (angle > 360.0f) { \
		angle -= 360.0f;\
	} \
	while (angle < -360.0f) { \
		angle += 360.0f; \
	} \
} \

#define calculVector(a, b, expression) XMStoreFloat3(&a, XMLoadFloat3(&a) expression b)

//###############################################################################
// CONVERSION GUIDELINES:
//
// In DirectXMath, you work with two kinds of types:
// 
// 1. Storage Types (e.g., XMFLOAT3, XMFLOAT4, XMFLOAT4X4):
//    - Used to store vector, quaternion, and matrix data persistently.
//    - Suitable for passing data between systems, saving to disk, or interfacing
//      with non-SIMD code.
//    - Example: In function parameters, you might pass an object's position
//      as an XMFLOAT3 or its transformation matrix as an XMFLOAT4X4.
// 
// 2. SIMD Types (e.g., XMVECTOR, XMMATRIX):
//    - Optimized for fast mathematical operations using SIMD instructions.
//    - Used within functions that perform high-performance calculations such as
//      rotations, translations, and scaling.
// 
// When and how to convert:
// 
// - Use XMLoadFloat3 or XMLoadFloat4 to convert from a storage type (XMFLOAT3/XMFLOAT4)
//   to a SIMD type (XMVECTOR) when you need to perform vector math.
// 
// - Use XMStoreFloat3 or XMStoreFloat4 to convert from an XMVECTOR back to an XMFLOAT3/XMFLOAT4
//   when you need to store or output the results of your computations.
// 
// - For matrices, use XMStoreFloat4x4 to store an XMMATRIX result into an XMFLOAT4X4 after performing
//   transformation calculations.
// 
// Example Contexts:
// 
// - In function parameters: If a function performs transformations, it might accept positions
//   as XMFLOAT3 for storage, but load them into XMVECTOR for internal calculations.
// 
// - In update functions: When updating an object's transformation, load its position (XMFLOAT3) into
//   an XMVECTOR, perform the required operations (rotation, scaling, etc.), then store the result back
//   into an XMFLOAT3. Similarly, for matrices, load an XMFLOAT4X4 into an XMMATRIX, compute the transformation,
//   and store the result back.
// 
// These conversion routines are essential for bridging the gap between the efficient SIMD math
// provided by DirectXMath and the simpler, more manageable data types used for storage and interface.
//
//More info: https://docs.microsoft.com/en-us/windows/win32/dxmath/conversion-functions
//
//IA explanation
//###############################################################################

void Transform::Identity() {

	vPosition = { 0.f, 0.f, 0.f };

	vForward = { 0.f, 0.f, 1.f };
	vRight = { 1.f, 0.f, 0.f };
	vUp = { 0.f, 1.f, 0.f };
	qRotation = { 0.f, 0.f, 0.f, 1.f };
	vOrigin = {0.f, 0.f, 0.f};
	
	vCachedLocalRotation = { 0.f, 0.f, 0.f };
	vCachedWorldRotation = { 0.f, 0.f, 0.f };

	vScale = { 1.f, 1.f, 1.f };

	this->Identity(mPosition);
	this->Identity(mRotation);
	this->Identity(mScale);
	this->Identity(mWorld);
}

void Transform::Identity(XMFLOAT4X4& matrix) {
	memset(&matrix, 0.f, sizeof(XMFLOAT4X4));

	matrix._11 = 1.f;
	matrix._22 = 1.f;
	matrix._33 = 1.f;
	matrix._44 = 1.f;
}

void Transform::SetPosition(const float x, const float y, const float z) {
	this->SetPosition(XMFLOAT3{ x, y, z });
}

void Transform::SetPosition(const XMFLOAT3& position) {
	vPosition = position;
	this->UpdatePositionMatrix();
}

void Transform::SetScale(const float x, const float y, const float z) {
	this->SetScale(XMFLOAT3{ x, y, z });
}

void Transform::SetScale(const XMFLOAT3& scale) {
	vScale = scale;
	this->UpdateScaleMatrix();
}

void Transform::SetScale(const XMVECTOR& scale) {
	XMStoreFloat3(&vScale, scale);
	this->UpdateScaleMatrix();
}

//#############################################################################
//################################## ROTATION #################################
//#############################################################################

void Transform::SetWorldRotation(float pitch, float yaw, float roll) {

	NormalizeDegreeAngle(pitch);
	NormalizeDegreeAngle(yaw);
	NormalizeDegreeAngle(roll);

	vCachedWorldRotation = { pitch, yaw, roll }; // Store the rotation angles in degrees

	//Format the roll to use the clockwise rotation system Left to Right
	roll = ToClockWiseRotationFormat(roll);

	// Convert the angles to radians
	const float pitchRad = XMConvertToRadians(pitch);
	const float yawRad = XMConvertToRadians(yaw);
	const float rollRad = XMConvertToRadians(roll);

	// Create a quaternion representing the rotation from the given *radians* angles
	const XMVECTOR newQuat = XMQuaternionRotationRollPitchYaw(pitchRad, yawRad, rollRad);
	XMStoreFloat4(&qRotation, newQuat);

	// Update the forward, right, and up vectors based on the new rotation
	const XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	const XMVECTOR defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	const XMVECTOR defaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	const XMVECTOR rotatedForward = XMVector3Rotate(defaultForward, newQuat);
	const XMVECTOR rotatedRight = XMVector3Rotate(defaultRight, newQuat);
	const XMVECTOR rotatedUp = XMVector3Rotate(defaultUp, newQuat);

	// Store the rotated vectors back into the member variables
	XMStoreFloat3(&vForward, rotatedForward);
	XMStoreFloat3(&vRight, rotatedRight);
	XMStoreFloat3(&vUp, rotatedUp);

	// Clean the rotation vectors to prevent floating-point errors
	CleanRotationVectors();

	UpdateRotationMatrix();
}

/*void Transform::WorldRotate(float pitch, float yaw, float roll) {

	std::cout << "WorldRotate: " << pitch << ", " << yaw << ", " << roll << std::endl;

	NormalizeDegreeAngle(pitch);
	NormalizeDegreeAngle(yaw);
	NormalizeDegreeAngle(roll);

	//Update the world rotation vector
	XMVECTOR vCachedRotationOld = XMLoadFloat3(&vCachedWorldRotation);
	vCachedRotationOld += { pitch, yaw, roll };
	XMStoreFloat3(&vCachedWorldRotation, vCachedRotationOld);
	NormalizeDegreeAngle(vCachedWorldRotation.x);
	NormalizeDegreeAngle(vCachedWorldRotation.y);
	NormalizeDegreeAngle(vCachedWorldRotation.z);

	//Format the roll to use the clockwise rotation system Left to Right
	roll = ToClockWiseRotationFormat(roll);

	// Convert angles to radians
	const float pitchRad = XMConvertToRadians(pitch);
	const float yawRad = XMConvertToRadians(yaw);
	const float rollRad = XMConvertToRadians(roll);

	// Create quaternions for rotation around the world axes
	XMVECTOR newQuat = XMQuaternionRotationRollPitchYaw(pitchRad, yawRad, rollRad);
	XMQuaternionRotationAxis({0, 1, 0}, yawRad);
	XMQuaternionRotationAxis({0, 0, 1}, rollRad);
	XMQuaternionRotationAxis({1, 0, 0}, pitchRad);

	// Load the current rotation quaternion
	XMVECTOR currentQuat = XMLoadFloat4(&qRotation);
	
	XMVECTOR combinedQuat = XMQuaternionMultiply(currentQuat, newQuat);

	// Normalize the combined quaternion
	combinedQuat = XMQuaternionNormalize(combinedQuat);
	XMStoreFloat4(&qRotation, combinedQuat);

	// Update direction vectors (forward, right, up)
	XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR defaultUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMVECTOR rotatedForward = XMVector3Rotate(defaultForward, combinedQuat);
	XMVECTOR rotatedRight = XMVector3Rotate(defaultRight, combinedQuat);
	XMVECTOR rotatedUp = XMVector3Rotate(defaultUp, combinedQuat);

	XMStoreFloat3(&vForward, rotatedForward);
	XMStoreFloat3(&vRight, rotatedRight);
	XMStoreFloat3(&vUp, rotatedUp);

	// Clean the rotation vectors to prevent floating-point errors
	CleanRotationVectors();

	// Update the rotation matrix and mark the transform matrix as dirty
	UpdateRotationMatrix();
}*/
void Transform::WorldRotate(float pitch, float yaw, float roll) {

	std::cout << "WorldRotate: " << pitch << ", " << yaw << ", " << roll << std::endl;

	NormalizeDegreeAngle(pitch);
	NormalizeDegreeAngle(yaw);
	NormalizeDegreeAngle(roll);

	//Update the world rotation vector
	XMVECTOR vCachedRotationOld = XMLoadFloat3(&vCachedWorldRotation);
	vCachedRotationOld += { pitch, yaw, roll };
	XMStoreFloat3(&vCachedWorldRotation, vCachedRotationOld);
	NormalizeDegreeAngle(vCachedWorldRotation.x);
	NormalizeDegreeAngle(vCachedWorldRotation.y);
	NormalizeDegreeAngle(vCachedWorldRotation.z);

	//Format the roll to use the clockwise rotation system Left to Right
	roll = ToClockWiseRotationFormat(roll);

	// Convert angles to radians
	const float pitchRad = XMConvertToRadians(pitch);
	const float yawRad = XMConvertToRadians(yaw);
	const float rollRad = XMConvertToRadians(roll);

	XMVECTOR qNewQuat = XMQuaternionRotationAxis({0, 1, 0}, yawRad); //Rotation Y
	XMVECTOR qRotTemp = XMQuaternionRotationAxis({1, 0, 0}, pitchRad); //Rotation X
	qNewQuat = XMQuaternionMultiply(qNewQuat, qRotTemp); //Y * X
	
	qRotTemp = XMQuaternionRotationAxis({0, 0, 1}, rollRad); //Rotation Z
	qNewQuat = XMQuaternionMultiply(qNewQuat, qRotTemp);

	XMVECTOR currentQuat = XMLoadFloat4(&qRotation);
	
	XMVECTOR combinedQuat = XMQuaternionMultiply(currentQuat, qNewQuat);
	XMStoreFloat4(&qRotation, combinedQuat);

	// Clean the rotation vectors to prevent floating-point errors
	CleanRotationVectors();

	// Update the rotation matrix and mark the transform matrix as dirty
	UpdateRotationMatrix();

	vRight.x = mRotation._11;
	vRight.y = mRotation._12;
	vRight.z = mRotation._13;
	vUp.x = mRotation._21;
	vUp.y = mRotation._22;
	vUp.z = mRotation._23;
	vForward.x = mRotation._31;
	vForward.y = mRotation._32;
	vForward.z = mRotation._33;
}

void Transform::RotateWorldPitch(const float angle) { this->WorldRotate(angle, 0.f, 0.f); }
void Transform::RotateWorldYaw(const float angle) { this->WorldRotate(0.f, angle, 0.f); }
void Transform::RotateWorldRoll(const float angle) { this->WorldRotate(0.f, 0.f, angle); }

void Transform::SetLocalRotation(float pitch, float yaw, float roll) {

	NormalizeDegreeAngle(pitch);
    NormalizeDegreeAngle(yaw);
    NormalizeDegreeAngle(roll);
	
    vCachedLocalRotation = { pitch, yaw, roll };

    //Format the roll to use the clockwise rotation system Left to Right
    roll = ToClockWiseRotationFormat(roll);

    // Convert angles to radians
    const float pitchRad = XMConvertToRadians(pitch);
    const float yawRad = XMConvertToRadians(yaw);
    const float rollRad = XMConvertToRadians(roll);

    XMVECTOR qNewQuat = XMQuaternionRotationAxis({0.f, 1.f, 0.f}, yawRad); //Rotate around Y

	//Reconstruct the right vector from the quaternion
	static const XMVECTOR defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR newTempRightVect = XMVector3Rotate(defaultRight, qNewQuat);

	XMVECTOR qRotTemp = XMQuaternionRotationAxis(newTempRightVect, pitchRad); //Rotate around X
	qNewQuat = XMQuaternionMultiply(qNewQuat, qRotTemp);

	//Reconstruct the forward vector from the quaternion
	static const XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR newTempForwardVect = XMVector3Rotate(defaultForward, qNewQuat);

	qRotTemp = XMQuaternionRotationAxis(newTempForwardVect, rollRad); //Rotate around Z
	qNewQuat = XMQuaternionMultiply(qNewQuat, qRotTemp);

    XMStoreFloat4(&qRotation, qNewQuat);

    // Clean the rotation vectors to prevent floating-point errors
    CleanRotationVectors();

    // Update the rotation matrix and mark the transform matrix as dirty
    UpdateRotationMatrix();

	vRight.x = mRotation._11;
	vRight.y = mRotation._12;
	vRight.z = mRotation._13;
	vUp.x = mRotation._21;
	vUp.y = mRotation._22;
	vUp.z = mRotation._23;
	vForward.x = mRotation._31;
	vForward.y = mRotation._32;
	vForward.z = mRotation._33;
}

void Transform::LocalRotatePitch(const float angle) { this->LocalRotate(angle, 0.f, 0.f); }
void Transform::LocalRotateYaw(const float angle) { this->LocalRotate(0.f, angle, 0.f); }
void Transform::LocalRotateRoll(const float angle) { this->LocalRotate(0.f, 0.f, angle); }
void Transform::LocalRotate(float pitch, float yaw, float roll) {

	NormalizeDegreeAngle(pitch);
	NormalizeDegreeAngle(yaw);
	NormalizeDegreeAngle(roll);
	
	//Update the world rotation vector
	XMVECTOR vCachedRotationOld = XMLoadFloat3(&vCachedLocalRotation);
	vCachedRotationOld += { pitch, yaw, roll };
	XMStoreFloat3(&vCachedLocalRotation, vCachedRotationOld);
	
	NormalizeDegreeAngle(vCachedLocalRotation.x);
	NormalizeDegreeAngle(vCachedLocalRotation.y);
	NormalizeDegreeAngle(vCachedLocalRotation.z);

	//Format the roll to use the clockwise rotation system Left to Right
	roll = ToClockWiseRotationFormat(roll);

	// Convert angles to radians
	const float pitchRad = XMConvertToRadians(pitch);
	const float yawRad = XMConvertToRadians(yaw);
	const float rollRad = XMConvertToRadians(roll);

	XMVECTOR qNewQuat = XMQuaternionRotationAxis(XMLoadFloat3(&vUp), yawRad); //Rotate around Y
	//Reconstruct the right vector from the quaternion
	static const XMVECTOR defaultRight = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR newTempRightVect = XMVector3Rotate(defaultRight, qNewQuat);

	XMVECTOR qRotTemp = XMQuaternionRotationAxis(newTempRightVect, pitchRad); //Rotate around X
	
	qNewQuat = XMQuaternionMultiply(qNewQuat, qRotTemp);
	//Reconstruct the forward vector from the quaternion
	static const XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR newTempForwardVect = XMVector3Rotate(defaultForward, qNewQuat);
	
	qRotTemp = XMQuaternionRotationAxis(newTempForwardVect, rollRad); //Rotate around Z
	qNewQuat = XMQuaternionMultiply(qNewQuat, qRotTemp);

	qNewQuat = XMQuaternionMultiply(XMLoadFloat4(&qRotation), qNewQuat);

	XMStoreFloat4(&qRotation, qNewQuat);

	// Clean the rotation vectors to prevent floating-point errors
	CleanRotationVectors();

	// Update the rotation matrix and mark the transform matrix as dirty
	UpdateRotationMatrix();
	
	vRight.x = mRotation._11;
	vRight.y = mRotation._12;
	vRight.z = mRotation._13;
	vUp.x = mRotation._21;
	vUp.y = mRotation._22;
	vUp.z = mRotation._23;
	vForward.x = mRotation._31;
	vForward.y = mRotation._32;
	vForward.z = mRotation._33;
}

void Transform::UpdateRotationFromTransform(const Transform& anotherTransform) {

	mRotation = anotherTransform.mRotation;
	qRotation = anotherTransform.qRotation;
	vCachedLocalRotation = anotherTransform.vCachedLocalRotation;
	vCachedWorldRotation = anotherTransform.vCachedWorldRotation;

	vForward = anotherTransform.vForward;
	vRight = anotherTransform.vRight;
	vUp = anotherTransform.vUp;

	bTransformMatrixDirty = true; 
}

float Transform::ToClockWiseRotationFormat(float angle) {

	//Reverse the angle to use the clockwise rotation system 
	return 0.f - angle;
}

void Transform::CleanRotationVectors() {
	
	constexpr float EPSILON = 0.00001f;
	constexpr float ONE_EPSILON = 1.0f - EPSILON;
	constexpr float MINUS_ONE_EPSILON = -1.0f + EPSILON;

	//Clean the values
	auto cleanValue = [EPSILON, ONE_EPSILON, MINUS_ONE_EPSILON](float& value) {
		if (fabs(value) < EPSILON) {
			value = 0.0f;
		}
		else if (value > ONE_EPSILON) {
			value = 1.0f;
		}
		else if (value < MINUS_ONE_EPSILON) {
			value = -1.0f;
		}
	};

	// Clean Forward vector
	cleanValue(vForward.x);
	cleanValue(vForward.y);
	cleanValue(vForward.z);

	// Clean Right vector
	cleanValue(vRight.x);
	cleanValue(vRight.y);
	cleanValue(vRight.z);

	// Clean Up vector
	cleanValue(vUp.x);
	cleanValue(vUp.y);
	cleanValue(vUp.z);
}

//#############################################################################
//################################## MATRIX ###################################
//#############################################################################


void Transform::UpdatePositionMatrix() {

	mPosition._14 = vPosition.x;
	mPosition._24 = vPosition.y;
	mPosition._34 = vPosition.z;

	bTransformMatrixDirty = true;
}

void Transform::UpdateRotationMatrix() {

	const XMVECTOR quat = XMLoadFloat4(&qRotation);

	// Convert the quaternion to a rotation matrix
	const XMMATRIX rotMat = XMMatrixRotationQuaternion(quat);

	XMStoreFloat4x4(&mRotation, rotMat);

	bTransformMatrixDirty = true;
}

void Transform::UpdateScaleMatrix() {

	this->Identity(mScale);

	mScale.m[0][0] = vScale.x;
	mScale.m[1][1] = vScale.y;
	mScale.m[2][2] = vScale.z;

	bTransformMatrixDirty = true;
}

void Transform::UpdateTransformMatrix() {

	if (bTransformMatrixDirty) {
		XMStoreFloat4x4(&mWorld,
			XMMatrixAffineTransformation(
				XMLoadFloat3(&vScale),
				XMLoadFloat3(&vOrigin),
				XMLoadFloat4(&qRotation),
				XMLoadFloat3(&vPosition)
			)
		);
		
		//XMStoreFloat4x4(&mTransform, XMLoadFloat4x4(&mPosition) * XMLoadFloat4x4(&mRotation) * XMLoadFloat4x4(&mScale));

		bTransformMatrixDirty = false;
	}
}

//#############################################################################
//################################## PRINT ####################################
//#############################################################################

std::string Transform::Print(const bool position, const bool rotation, const bool scale, const bool transform, const bool matrix, const bool quaternion) const {

	std::ostringstream oss;
#ifdef _DEBUG
	oss << std::fixed << std::setprecision(2); // Set the precision of the output to 2 decimal places and fixed notation (no scientific notation eg: 0e-17)
	oss << std::endl << std::endl;
	oss << "==============> TRANSFORM <==============" << std::endl;
	oss << std::endl;
	// === World Space Position ===
	if (position) {
		oss << "(=== World Space Position ===)" << std::endl;
		oss << "Position: (" << vPosition.x << ", " << vPosition.y << ", " << vPosition.z << ")" << std::endl;

		if (matrix) {
			oss << "Position Matrix: " << std::endl;
			oss << mPosition._11 << " " << mPosition._12 << " " << mPosition._13 << " " << mPosition._14 << std::endl;
			oss << mPosition._21 << " " << mPosition._22 << " " << mPosition._23 << " " << mPosition._24 << std::endl;
			oss << mPosition._31 << " " << mPosition._32 << " " << mPosition._33 << " " << mPosition._34 << std::endl;
			oss << mPosition._41 << " " << mPosition._42 << " " << mPosition._43 << " " << mPosition._44 << std::endl;
		}
		oss << std::endl;
	}
	// === Local Space Rotation ===
	if (rotation) {
		oss << "(=== Local Space Rotation ===)" << std::endl;
		oss << std::setprecision(5);
		oss << "# Left Hand System and YXZ order. #" << std::endl;
		oss << "World Rotation in degrees: (" << vCachedWorldRotation.x << ", " << vCachedWorldRotation.y << ", " << vCachedWorldRotation.z << ")" << std::endl;
		oss << "Local Rotation in degrees: (" << vCachedLocalRotation.x << ", " << vCachedLocalRotation.y << ", " << vCachedLocalRotation.z << ")" << std::endl;
		oss << "Forward Vector: (" << vForward.x << ", " << vForward.y << ", " << vForward.z << ")" << std::endl;
		oss << "Right Vector: (" << vRight.x << ", " << vRight.y << ", " << vRight.z << ")" << std::endl;
		oss << "Up Vector: (" << vUp.x << ", " << vUp.y << ", " << vUp.z << ")" << std::endl;

		if (quaternion) {
			oss << "Rotation Quaternion: (" << qRotation.x << ", " << qRotation.y << ", " << qRotation.z << ", " << qRotation.w << ")" << std::endl;
		}

		if (matrix) {
			oss << "Rotation Matrix: " << std::endl;
			oss << mRotation._11 << " " << mRotation._12 << " " << mRotation._13 << " " << mRotation._14 << std::endl;
			oss << mRotation._21 << " " << mRotation._22 << " " << mRotation._23 << " " << mRotation._24 << std::endl;
			oss << mRotation._31 << " " << mRotation._32 << " " << mRotation._33 << " " << mRotation._34 << std::endl;
			oss << mRotation._41 << " " << mRotation._42 << " " << mRotation._43 << " " << mRotation._44 << std::endl;
		}
		oss << std::setprecision(2);
		oss << std::endl;
	}

	// === Scale ===
	if (scale) {
		oss << "(=== Scale ===)" << std::endl;
		oss << "Scale: (" << vScale.x << ", " << vScale.y << ", " << vScale.z << ")" << std::endl;

		if (matrix) {
			oss << "Scale Matrix: " << std::endl;
			oss << mScale._11 << " " << mScale._12 << " " << mScale._13 << " " << mScale._14 << std::endl;
			oss << mScale._21 << " " << mScale._22 << " " << mScale._23 << " " << mScale._24 << std::endl;
			oss << mScale._31 << " " << mScale._32 << " " << mScale._33 << " " << mScale._34 << std::endl;
			oss << mScale._41 << " " << mScale._42 << " " << mScale._43 << " " << mScale._44 << std::endl;
		}
		oss << std::endl;
	}

	// === Transform ===
	if (transform) {
		oss << "(=== Transform ===)" << std::endl;
		oss << "Transform Matrix: " << std::endl;
		oss << mWorld._11 << " " << mWorld._12 << " " << mWorld._13 << " " << mWorld._14 << std::endl;
		oss << mWorld._21 << " " << mWorld._22 << " " << mWorld._23 << " " << mWorld._24 << std::endl;
		oss << mWorld._31 << " " << mWorld._32 << " " << mWorld._33 << " " << mWorld._34 << std::endl;
		oss << mWorld._41 << " " << mWorld._42 << " " << mWorld._43 << " " << mWorld._44 << std::endl;
	}
	oss << "=========================================" << std::endl;
#endif
	return oss.str();

}

std::ostream& operator<<(std::ostream& os, const Transform& transform) {
	os << transform.Print(true, true, true, true, true, true);
	return os;
}

//#############################################################################
//################################## TOOLS ####################################
//#############################################################################

XMFLOAT3 Transform::ExtractPitchYawRollFromMatrix(const XMMATRIX& matrix) {
    
	XMFLOAT4X4 mat;
	XMStoreFloat4x4(&mat, matrix);

	XMFLOAT3 angles;
	
	float sp = -mat._32;
	if (sp <= -1.0f) {
		angles.x = -90.0f;
	} else if (sp >= 1.0f) {
		angles.x = 90.0f;
	} else {
		angles.x = XMConvertToDegrees(asinf(sp));
	}
	
	if (abs(sp) > 0.9999f) {
		angles.y = XMConvertToDegrees(atan2f(-mat._13, mat._11));
		angles.z = 0.0f;
	} else {
		angles.y = XMConvertToDegrees(atan2f(mat._31, mat._33));
		angles.z = XMConvertToDegrees(atan2f(mat._12, mat._22));
	}
	
	NormalizeDegreeAngle(angles.x);
	NormalizeDegreeAngle(angles.y);
	NormalizeDegreeAngle(angles.z);

	return angles;
}