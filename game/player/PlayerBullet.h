#pragma once
#include"../../engine/bonjin/BonjinEngine.h"


class PlayerBullet {
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

	const float kRadius = 2.0f;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerBullet();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Camera* camera);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	bool IsDead()const { return isDead_; }

	/// <summary>
	/// 衝突を検知したら呼び出される
	/// </summary>
	void OnCollision();

	float GetRadius()const { return kRadius; }

	Vector3 GetWorldPosition();
};

