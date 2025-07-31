#pragma once
#include"../../engine/bonjin/BonjinEngine.h"
#include<list>

#include"PlayerBullet.h"
#include"../utility/Collider.h"

class Player : public Collider {
private:
	// モデル
	Model* model_ = nullptr;

	// ワールド変換
	WorldTransform worldTransform_{};

	// 弾
	std::list<PlayerBullet*> bullets_;
	std::list<Model*> bulletModel_;

	const float kRadius = 2.0f;

	// レティクル
	WorldTransform worldTransform3DReticle_;
	Model* reticleModel_ = nullptr;
	Sprite* sprite2DReticle_ = nullptr;

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// 旋回処理
	/// </summary>
	void Rotate();

	/// <summary>
	/// 攻撃処理
	/// </summary>
	void Attack();

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, Vector3 position);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Camera* camera);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	Vector3 GetWorldPosition()override;

	Vector3 GetReticleWorldPosition();

	/// <summary>
	/// 衝突を検知したら呼び出される
	/// </summary>
	void OnCollision()override;

	const std::list<PlayerBullet*>& GetBullets()const { return bullets_; }

	float GetRadius()const { return kRadius; }

	void SetParent(WorldTransform* parent) { worldTransform_.parent = parent; }

};

