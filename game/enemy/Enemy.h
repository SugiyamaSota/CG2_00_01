#pragma once
#include"../../engine/bonjin/BonjinEngine.h"

#include"../others/Data.h"

class Enemy {
private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	// カメラ
	Camera* camera_ = nullptr;

	// 3Dモデル
	Model* model_ = nullptr;

	static inline const float kWidth_ = 2.0f;  // 当たり判定の幅
	static inline const float kHeight_ = 2.0f; // 当たり判定の高さ

	LRDirection lrDirection_ = LRDirection::kLeft; // モデルの向き

	///// 移動関連 /////
	Vector3 velocity_ = {};          // 速度保管

	static inline const float kWalkSpeed = 0.05f; //歩行速度

	static inline const float kWalkMotionAngleStart = -25.0f; //首を振る最初の角度
	static inline const float kWalkMotionAngleEnd = 50.0f;   //首を振る最後の角度
	static inline const float kWalkTimer = 2.0f;  //首を振る周期
	float walkTimer_ = 0.0f;
	static inline const float pi = float(3.14159265359);

	// ロックオン状態
	bool isLockedOn_ = false;

	// 死亡状態
	bool isDead_ = false;
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// モデルの向きの制御
	/// </summary>
	void TurningControl();

	Vector3 GetWorldPosition();

	AABB GetAABB();

	void OnCollision();

	bool GetIsLockedOn() { return isLockedOn_; }
	void SetIsLockedOn(bool frag);

	bool GetIsDead() const { return isDead_; }
	void SetIsDead(bool frag) { isDead_ = frag; }
};