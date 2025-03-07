#pragma once

struct Entity;

struct CollidingResult {
	bool result = false;
	DirectX::XMFLOAT3 normal = {0.f, 0.f, 0.f};
	Entity* entityCollided = nullptr;
	float penetrationDepth = 0.f;
};

std::ostream& operator<<(std::ostream& os, const CollidingResult& collidingResult);