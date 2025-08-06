#pragma once
#include "../../engine/bonjin/BonjinEngine.h"

#include <cassert>
#include<memory>

#include"Anchor.h"
#include"../others/Data.h"

class MapChipField;
class Enemy;

/// <summary>
/// 自キャラ
/// </summary>
class Player {
private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	// カメラ
	Camera* camera_ = nullptr;

	// 3Dモデル
	Model* model_ = nullptr;

	// 移動速度
	Vector3 velocity_ = {};
	static inline const float kAcceleration = 0.010f;
	static inline const float kAttenuation = 0.5f;
	static inline const float kLimitRunSpeed = 0.2f;

	// 向き
	LRDirection lrDirection_ = LRDirection::kRight;

	//旋回関連
	float turnFirstRotationY_ = 0.0f;
	float turnTimer_ = 0.0f;
	static inline const float kTimeTurn = 0.3f;

	//重力加速度
	static inline const float kGravityAcceleration = 0.0098f;
	//最大落下速度
	static inline const float kLimitFallSpeed = 2.0f;
	//ジャンプ初速
	static inline const float kJumpAcceleration = 0.3f;

	//マップチップフィールド
	MapChipField* mapChipField_ = nullptr;

	// --- 当たり判定 ---
	// サイズ
	static inline const float kWidth = 2.0f;
	static inline const float kHeight = 2.0f;

	// 衝突後処理の数値
	static inline const float kAttenuationTop = 0.5f;
	static inline const float kAttenuationWall = 0.5f;

	//接地状態フラグ
	bool onGround_ = true;

	// --- アンカー ---
	std::unique_ptr<Anchor> anchor_;
	/// <summary>
	/// アンカーの射出
	/// </summary>
	void shootAnchor();
	
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, Camera* camera, const Vector3& position);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();


	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// 4方向の当たり判定
	/// </summary>
	/// <param name="info">衝突情報</param>
	void isCollisionMapTop(CollisionMapInfo& info);
	void isCollisionMapBottom(CollisionMapInfo& info);
	void isCollisionMapLeft(CollisionMapInfo& info);
	void isCollisionMapRight(CollisionMapInfo& info);
	
	/// <summary>
	/// 当たり判定をまとめる関数
	/// </summary>
	/// <param name="info">衝突判定</param>
	void isCollisionMap(CollisionMapInfo& info);

	/// <summary>
	/// 着地判定
	/// </summary>
	/// <param name="info">衝突情報</param>
	void CheckLanding(const CollisionMapInfo& info);

	/// <summary>
	/// 衝突判定による影響の処理
	/// </summary>
	/// <param name="info">衝突情報</param>
	void ResolveCollision(const CollisionMapInfo& info);

	/// <summary>
	/// 衝突フラグの処理(天井)
	/// </summary>
	/// <param name="info">衝突情報</param>
	void TopCollisionReaction(const CollisionMapInfo& info);

	/// <summary>
	/// 衝突フラグの処理(壁)
	/// </summary>
	/// <param name="info">衝突情報</param>
	void WallCollisionReaction(const CollisionMapInfo& info);

	Vector3 CornerPosition(const Vector3& center, Corner corner);

	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }
	const Vector3& GetVelocity() const { return velocity_; }
	WorldTransform& GetWorldTransform() { return worldTransform_; }
	Vector3 GetPosition()const { return worldTransform_.translate; }
	Vector3 GetWorldPosition();
	AABB GetAABB();
	void OnCollision(const Enemy* enemy);
	bool HasAnchor() const {
		return anchor_ != nullptr;
	}

	Anchor& GetAnchor() {
		assert(anchor_); // nullではないことを確認
		return *anchor_;
	}

	/// <summary>
	/// アンカーのゲッター (const版)
	/// </summary>
	const Anchor& GetAnchor() const {
		assert(anchor_); // nullではないことを確認
		return *anchor_;
	}
};