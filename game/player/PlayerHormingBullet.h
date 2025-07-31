#pragma once
#include"../../engine/bonjin/BonjinEngine.h"

#include"../utility/Collider.h"
#include"../enemy/Enemy.h" // Enemy クラスの定義が必要なため追加

class PlayerHormingBullet : public Collider {
private:
	// モデル
	Model* model_ = nullptr;

	// ワールド変換
	WorldTransform worldTransform_;

	// 速度
	Vector3 velocity_;

	// ホーミング対象
	Enemy* target_ = nullptr; // ホーミング対象の敵を追加

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
	// スケールとターゲット引数を追加
	void Initialize(Model* model, const Vector3& position, const Vector3& velocity, const Vector3& scale, Enemy* target); //

	/// <summary>
	/// デストラクタ
	/// </summary>
	~PlayerHormingBullet();

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
	void OnCollision()override;

	float GetRadius()const { return kRadius; }

	Vector3 GetWorldPosition()override;
};