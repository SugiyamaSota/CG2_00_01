#pragma once
#include"../../engine/bonjin/BonjinEngine.h"

class EnemyBullet {
private:
	// モデル
	Model* model_ = nullptr;

	// ワールド変換
	WorldTransform worldTransform_;

	// 速度
	Vector3 velocity_;

	// 寿命
	static const int32_t kLifeTime = 60 * 5;

	// デスタイマー
	uint32_t deathTimer_ = kLifeTime;

	// デスフラグ
	bool isDead_ = false;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

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
};
