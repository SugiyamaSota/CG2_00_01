#pragma once
#include"../../engine/bonjin/BonjinEngine.h"
#include"../utility/Collider.h"

class Player;

class EnemyBullet : public Collider {
private:
	// モデル
	Model* model_ = nullptr;

	// ワールド変換
	WorldTransform worldTransform_;

	Matrix4x4 worldMatrix_;

	// 速度
	Vector3 velocity_;

	// 寿命
	static const int32_t kLifeTime = 60 * 5;

	// デスタイマー
	uint32_t deathTimer_ = kLifeTime;

	// デスフラグ
	bool isDead_ = false;

	Player* player_ = nullptr;

	const float kRadius = 2.0f;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity,Player& player);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~EnemyBullet();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Camera* camera);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	bool IsDead()const { return isDead_; }

	void SetPlayer(Player* player) { player_ = player; }

	Vector3 GetWorldPosition()override;

	/// <summary>
	/// 衝突を検知したら呼び出される
	/// </summary>
	void OnCollision()override;

	float GetRadius()const { return kRadius; }
};
