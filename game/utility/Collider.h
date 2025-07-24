#pragma once
#include"../../engine/bonjin/BonjinEngine.h"

#include"CollisionConfig.h"

class Collider {
private:
	// 衝突半径
	float radius_ = 2.0f;

	// 衝突属性(自分)
	uint32_t collisionAttibute_ = 0xffffffff;

	// 衝突属性(自分)
	uint32_t collisionMask_ = 0xffffffff;
public:
	float GetRadius()const { return radius_; }

	void SetRadius(float radius) { radius_ = radius; }

	virtual void OnCollision();

	virtual Vector3 GetWorldPosition() = 0;

	virtual ~Collider() = default;

	// 衝突属性(自分)の関数
	uint32_t GetCollisionAttibute() { return collisionAttibute_; }
	void SetCollisionAttibute(uint32_t collisionAttibute) { collisionAttibute_ = collisionAttibute; }

	// 衝突属性(相手)の関数
	uint32_t GetCollisionMask() { return collisionMask_; }
	void SetCollisionMask(uint32_t collisionMask) { collisionMask_ = collisionMask; }
};
