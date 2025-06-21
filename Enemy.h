#pragma once
#include"engine/bonjin/BonjinEngine.h"

class Enemy {
private:
	// 向いている方向の情報enum
	enum class LRDirection {
		kRight,
		kLeft,
	};

	WorldTransform worldTransform_; // ワールドトランスフォーム

	DebugCamera* camera_ = nullptr; // カメラ

	Model* model_ = nullptr; // 3Dモデル

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


public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, DebugCamera* camera, const Vector3& position);

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
};