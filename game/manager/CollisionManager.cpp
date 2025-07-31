#include "CollisionManager.h"
#include "../player/PlayerBullet.h" 
#include "../enemy/EnemyBullet.h"  
#include"../player/PlayerHormingBullet.h"

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

// Initializeの実装
void CollisionManager::Initialize(Player* player, std::list<Enemy*>* enemies) {
	player_ = player;
	enemies_ = enemies;
}

// Updateの実装
void CollisionManager::Update(const std::list<EnemyBullet*>& enemyBullets, const std::list<PlayerBullet*>& playerBullets, const std::list<PlayerHormingBullet*>& playerHormingBullets, const std::list<Enemy*>& enemies) {
	colliders_.clear();

	colliders_.push_back(player_);

	// すべての敵を追加
	for (Enemy* enemy : enemies) { // 引数で受け取った enemies を使用
		colliders_.push_back(enemy);
	}

	// プレイヤーの弾丸を追加
	for (PlayerBullet* bullet : playerBullets) { // 引数で受け取った playerBullets を使用
		colliders_.push_back(bullet);
	}

	for (PlayerHormingBullet* bullet : playerHormingBullets) {
		colliders_.push_back(bullet);
	}

	// 敵の弾丸を追加
	for (EnemyBullet* bullet : enemyBullets) { // 引数で受け取った enemyBullets を使用
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