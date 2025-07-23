#pragma once
#include"../../engine/bonjin/BonjinEngine.h"

class Collider {
private:
	// 衝突半径
	float radius_ = 2.0f;
public:
	float GetRadius()const { return radius_; }

	void SetRadius(float radius) { radius_ = radius; }

	virtual void OnCollision();

	virtual Vector3 GetWorldPosition() = 0;

	virtual ~Collider() = default;
};
