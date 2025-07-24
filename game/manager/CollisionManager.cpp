#include "CollisionManager.h"

void CollisionManager::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	// フィルター
	if (
		(colliderA->GetCollisionAttibute() ^ colliderB->GetCollisionMask() ||
			colliderB->GetCollisionAttibute() ^ colliderA->GetCollisionMask())
		) {
		return;
	}

	Vector3 posA, posB;
	posA = colliderA->GetWorldPosition();
	posB = colliderB->GetWorldPosition();
	// 座標の差分
	Vector3 distance = posA - posB;
	float length = Length(distance);
	if (colliderA->GetRadius() + colliderB->GetRadius() > length) {
		colliderA->OnCollision();
		colliderB->OnCollision();
	}
}

void CollisionManager::Initialize(Player* player,Enemy*enemy) {
	player_ = player;
	enemy_ = enemy;

}

void CollisionManager::Update() {
	colliders_.clear();

	colliders_.push_back(player_);
	colliders_.push_back(enemy_);

	const std::list<PlayerBullet*>playerBullets = player_->GetBullets();
	const std::list<EnemyBullet*>enemyBullets = enemy_->GetBullets();

	for (PlayerBullet* bullet : playerBullets) {
		colliders_.push_back(bullet);
	}

	for (EnemyBullet* bullet : enemyBullets) {
		colliders_.push_back(bullet);
	}

	std::list<Collider*>::iterator itrA = colliders_.begin();

	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;

			CheckCollisionPair(colliderA, colliderB);
		}
	}
}
