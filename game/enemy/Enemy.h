#pragma once
#include"../../engine/bonjin/BonjinEngine.h"
#include"EnemyBullet.h"
#include <list>
#include<functional>
#include"../utility/TimedCall.h"
#include"../utility/Collider.h"

class Player;
class Enemy;
class GameScene; // GameSceneの前方宣言を追加

class BaseEnemyState {
public:
	virtual ~BaseEnemyState() = default;
	// Enemyオブジェクトへのポインタを渡すことで、Enemyのメンバにアクセスできるようにする
	virtual void Update(Enemy* enemy) = 0;
	// 必要に応じて、Enter()やExit()などの状態遷移時の処理を追加
};

// Concreteな状態クラスをBaseEnemyStateを継承してEnemyの内部に定義
class EnemyStateApproach : public BaseEnemyState {
public:
	void Update(Enemy* enemy) override;
};

class EnemyStateLeave : public BaseEnemyState {
public:
	void Update(Enemy* enemy) override;
};

class Enemy : public Collider {
private:
	Model* model_ = nullptr;
	WorldTransform worldTransform_;

	const float kMoveSpeed = 0.25f;
	const Vector3 kApproachSpeed = { 0,0,-kMoveSpeed };
	const Vector3 kLeaveSpeed = { -kMoveSpeed,0,0 };

	BaseEnemyState* state_ = nullptr; // 現在のEnemyの状態

	// Removed: std::list<EnemyBullet*> bullets_;
	// Removed: std::list<Model*> bulletModel_;

	std::function<void()> fireFunction_;

	std::list<TimedCall*>timedCalls_;

	Player* player_ = nullptr;
	GameScene* gameScene_ = nullptr; // 新しい弾丸についてGameSceneに通知するためのポインタ

	const float kRadius = 2.0f;

	bool isDead_ = false;

public:
	static const int kFireInterval = 60;

	// 状態クラスからアクセスされるためのpublicメソッド
	void MoveApproach() { worldTransform_.translate += kApproachSpeed; }
	void MoveLeave() { worldTransform_.translate += kLeaveSpeed; }
	Vector3 GetPosition() const { return worldTransform_.translate; }


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& startPosition);

	~Enemy();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Camera* camera);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	void ChangeState(BaseEnemyState* newState); // 状態を変更するメソッド

	void Fire();

	const std::function<void()>& GetFireFunction() const {
		return fireFunction_;
	}

	void FireAndReset();

	void SetPlayer(Player* player) { player_ = player; }
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; } // GameSceneのセッター

	Vector3 GetWorldPosition()override;

	/// <summary>
	/// 衝突を検知したら呼び出される
	/// </summary>
	void OnCollision()override;

	// Removed: const std::list<EnemyBullet*>& GetBullets()const { return bullets_; }

	float GetRadius()const { return kRadius; }

	bool IsDead()const { return isDead_; }
};