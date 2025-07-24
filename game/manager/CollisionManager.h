#pragma once

#include"../utility/Collider.h"
#include"../player/Player.h"
#include"../enemy/Enemy.h"

class CollisionManager{
private:
	// コライダーリスト
	std::list<Collider*>colliders_;

	// 対象
	Player* player_;
	Enemy* enemy_;

	void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

public:
	/// <summary>
	///  初期化
	/// </summary>
	void Initialize(Player* player, Enemy* enemy);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

};

