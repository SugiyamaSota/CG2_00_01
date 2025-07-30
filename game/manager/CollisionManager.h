#pragma once

#include"../utility/Collider.h"
#include"../player/Player.h"
#include"../enemy/Enemy.h"
#include <list> 

class EnemyBullet;
class PlayerBullet; // PlayerBullet の前方宣言を追加

class CollisionManager {
private:
	std::list<Collider*>colliders_;

	Player* player_;
	std::list<Enemy*>* enemies_ = nullptr; // Enemyポインタのリストのポインタ

	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

public:
	// Initializeの引数: Player*とstd::list<Enemy*>*
	void Initialize(Player* player, std::list<Enemy*>* enemies);

	// Updateの引数: 敵弾リスト、プレイヤー弾リスト、敵リスト
	void Update(const std::list<EnemyBullet*>& enemyBullets, const std::list<PlayerBullet*>& playerBullets, const std::list<Enemy*>& enemies);

};