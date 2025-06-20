#pragma once
#include "engine/bonjin/BonjinEngine.h"
#include <cassert>

/// <summary>
/// 自キャラ
/// </summary>
class Player {
private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	//カメラ
	DebugCamera* camera_ = nullptr;

	// 3Dモデル
	Model* model_ = nullptr;

	//移動
	Vector3 velocity_ = {};

	static inline const float kAcceleration = 0.1f;
	static inline const float kAttenuation = 0.1f;
	static inline const float kLimitRunSpeed = 5.0f;

	enum class LRDirection {
		kRight,
		kLeft,
	};

	LRDirection lrDirection_ = LRDirection::kRight;

	//旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	//旋回タイマー
	float turnTimer_ = 0.0f;
	//旋回時間(秒)
	static inline const float kTimeTurn = 0.3f;

	//接地状態フラグ
	bool onGround_ = true;

	//重力加速度
	static inline const float kGravityAcceleration = 0.98f;
	//最大落下速度
	static inline const float kLimitFallSpeed = 2.0f;
	//ジャンプ初速
	static inline const float kJumpAcceleration = 3.0f;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, DebugCamera* camera, Vector3 position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(InputKey* key);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	Vector3 GetPosition() { return worldTransform_.translate; }
};