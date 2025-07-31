#pragma once
#include"../../engine/bonjin/BonjinEngine.h"

class Player;
class Enemy;

class LockOn {
private:
	WorldTransform lockOnWorldTransform_;
	Sprite* lockOnSprite_ = nullptr;

	// 対象
	Enemy* target_ = nullptr;

public:
	void Initialize();

	~LockOn();

	void Update(Player* player, std::list<Enemy*>& enemies, const Camera& camera);

	void Draw();

	Enemy* GetTarget()const { return target_; }
};

