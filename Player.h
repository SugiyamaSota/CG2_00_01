#pragma once
#include "engine/bonjin/BonjinEngine.h"
#include <cassert>

class MapChipField;

/// <summary>
/// 自キャラ
/// </summary>
class Player {
private:
private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	//カメラ
	DebugCamera* camera_ = nullptr;

	// 3Dモデル
	Model* model_ = nullptr;


	//移動
	Vector3 velocity_ = {};

	static inline const float kAcceleration = 0.05f;
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
	static inline const float kGravityAcceleration = 0.0025f;
	//最大落下速度
	static inline const float kLimitFallSpeed = 2.0f;
	//ジャンプ初速
	static inline const float kJumpAcceleration = 0.2f;

	//マップチップフィールド
	MapChipField* mapChipField_ = nullptr;

	//当たり判定サイズ
	static inline const float kWidth = 2.0f;
	static inline const float kHeight = 2.0f;

	struct CollisionMapInfo {
		bool isHotTop_ = false;
		bool isLandin_ = false;
		bool isHitWall_ = false;
		Vector3 movement_;
	};

	enum Corner {
		kRightBottom,
		kLeftBottom,
		kRightTop,
		kLeftTop,

		kNumCorner
	};

	static inline const float kAttenuationTop = 0.5f;
	static inline const float kAttenuationWall = 0.5f;


public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, DebugCamera* camera, const Vector3& position);

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move(InputKey* key);

	void isCollisionMapTop(CollisionMapInfo& info);
	void isCollisionMapBottom(CollisionMapInfo& info);
	void isCollisionMapLeft(CollisionMapInfo& info);
	void isCollisionMapRight(CollisionMapInfo& info);

	void CheckLanding(const CollisionMapInfo& info);


	void ResolveCollision(const CollisionMapInfo& info);

	void TopCollisionReaction(const CollisionMapInfo& info);
	void WallCollisionReaction(const CollisionMapInfo& info);

	void isCollisionMap(CollisionMapInfo& info);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	/// <summary>
	/// 更新
	/// </summary>
	void Update(InputKey* key);



	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	const Vector3& GetVelocity() const { return velocity_; }
	WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3 GetPosition()const { return worldTransform_.translate; }
};