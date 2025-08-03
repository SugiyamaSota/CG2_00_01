#define NOMINMAX
#include "Player.h"
#include"../mapchip/MapChipField.h"
#include <algorithm>
#include <numbers>
#include<array>

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
	assert(model);
	model_ = model;

	worldTransform_ = {
		{1,1,1},
		{0,0,0},
		position,
	};

	camera_ = camera;

	onGround_ = false;

	isTeleported_ = false;
}

void Player::Move() {
	// 移動入力
	// //接地状態
	if (onGround_) {
		// 左右移動操作
		if (Input::GetInstance()->IsPress(DIK_D) || Input::GetInstance()->IsPress(DIK_A)) {
			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->IsPress(DIK_D)) {
				if (velocity_.x < 0.0f) {

					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x += kAcceleration;
				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;
					turnFirstRotationY_ = worldTransform_.rotate.y;
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->IsPress(DIK_A)) {
				if (velocity_.x > 0.0f) {

					// 速度と逆方向に入力中は急ブレーキ
					velocity_.x *= (1.0f - kAttenuation);
				}
				acceleration.x -= kAcceleration;
				if (lrDirection_ != LRDirection::kLeft) {
					lrDirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotate.y;
					turnTimer_ = kTimeTurn;
				}
			}
			// 加速/減速
			velocity_ = Add(velocity_, acceleration);

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 非入力時は移動減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (Input::GetInstance()->IsPress(DIK_SPACE)) {
			velocity_ = Add(velocity_, Vector3(0, kJumpAcceleration, 0));
		}
	} else {
		// 空中
		velocity_ = Add(velocity_, Vector3(0, -kGravityAcceleration, 0));
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

// 移動量を加味した衝突判定
void Player::isCollisionMap(CollisionMapInfo& info) {
	isCollisionMapLeft(info);
	isCollisionMapRight(info);
	isCollisionMapTop(info);
	isCollisionMapBottom(info);

}

// 天井の衝突判定
void Player::isCollisionMapTop(CollisionMapInfo& info) {
	std::array<Vector3, 4> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translate, info.movement_), static_cast<Corner>(i));
	}

	if (info.movement_.y <= 0) {
		return;
	}

	MapChipType mapChipType;
	// 真上の当たり判定
	bool hit = false;
	// 左上
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 左上
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
		info.movement_.y = std::max(0.0f, velocity_.y);
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
		info.movement_.y = std::max(0.0f, velocity_.y);
		// 天井に当たったことを記録
		info.isHotTop_ = true;
	}
}

void Player::isCollisionMapBottom(CollisionMapInfo& info) {
	std::array<Vector3, 4> positionsNew;

	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translate, info.movement_), static_cast<Corner>(i));
	}

	if (info.movement_.y >= 0) {
		return;
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真下の当たり判定
	bool hit = false;
	// 左下
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}
	// 右下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex - 1);
	if (mapChipType == MapChipType::kBlock && mapChipTypeNext != MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		Vector3 temporaryPosition = worldTransform_.translate;
		temporaryPosition.y -= kHeight / 2;
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(temporaryPosition);
		if (indexSetNow.yIndex != indexSet.yIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// 修正: プレイヤーの底面が地面の上端に乗るように
			info.movement_.y = std::max(0.0f, rect.top - (worldTransform_.translate.y - kHeight / 2));
			info.isLandin_ = true;
		}
	}
}

// 右の壁の衝突判定
void Player::isCollisionMapRight(CollisionMapInfo& info) {
	// 右移動しているか？
	if (info.movement_.x <= 0) {
		return;
	}

	// 四つの角の座標の取得
	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translate, info.movement_), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真下の当たり判定
	bool hit = false;
	// 右上
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex + 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックに当たっているか？
	if (hit) {
		// めり込みを排除する方向に移動量を設定
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kRightBottom]);
		Vector3 temporaryPosition = worldTransform_.translate;
		temporaryPosition.x += kWidth / 2;
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(temporaryPosition);
		if (indexSetNow.xIndex != indexSet.xIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// 修正: プレイヤーの底面が地面の上端に乗るように
			info.movement_.x = std::max(0.0f, rect.right - worldTransform_.translate.x - kWidth);
			info.isHitWall_ = true;
		}
	}
}

// 左の壁の衝突判定
void Player::isCollisionMapLeft(CollisionMapInfo& info) {
	// 左移動しているか？
	if (info.movement_.x >= 0) {
		return;
	}

	// 四つの角の座標の取得
	std::array<Vector3, 4> positionsNew;
	for (uint32_t i = 0; i < positionsNew.size(); ++i) {
		positionsNew[i] = CornerPosition(Add(worldTransform_.translate, info.movement_), static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	MapChipType mapChipTypeNext;
	// 真下の当たり判定
	bool hit = false;
	// 左上
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftTop]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 左下
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
	mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
	mapChipTypeNext = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex - 1, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	// ブロックに当たっているか？
	if (hit) {
		// めり込みを排除する方向に移動量を設定
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionsNew[kLeftBottom]);
		Vector3 temporaryPosition = worldTransform_.translate;
		temporaryPosition.x -= kWidth / 2;
		MapChipField::IndexSet indexSetNow;
		indexSetNow = mapChipField_->GetMapChipIndexSetByPosition(temporaryPosition);
		if (indexSetNow.xIndex != indexSet.xIndex) {
			MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
			// 修正: プレイヤーの底面が地面の上端に乗るように
			info.movement_.x = std::max(0.0f, rect.right - worldTransform_.translate.x + kWidth / 2);
			info.isHitWall_ = true;
		}
	}
}

void Player::CheckLanding(const CollisionMapInfo& info) {
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		} else {

			std::array<Vector3, 4> positionsNew;

			for (uint32_t i = 0; i < positionsNew.size(); ++i) {
				positionsNew[i] = CornerPosition(Add(worldTransform_.translate, info.movement_), static_cast<Corner>(i));
			}

			MapChipType mapChipType;
			// 真下の当たり判定
			bool hit = false;
			// 左下
			MapChipField::IndexSet indexSet;
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kLeftBottom], Vector3(0, -0.00001f, 0)));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			// 右下
			indexSet = mapChipField_->GetMapChipIndexSetByPosition(Add(positionsNew[kRightBottom], Vector3(0, -0.00001f, 0)));
			mapChipType = mapChipField_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			if (mapChipType == MapChipType::kBlock) {
				hit = true;
			}
			if (!hit) {
				onGround_ = false;
			}
		}
	} else {
		if (info.isLandin_) {
			onGround_ = true;
			velocity_.y *= (1.0f - 0.5f);
			velocity_.y = 0.0f;
		}
	}
}



// 角の座標取得
Vector3 Player::CornerPosition(const Vector3& center, Corner corner) {
	Vector3 offsetTable[kNumCorner] = {
		{+kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
		{-kWidth / 2.0f, -kHeight / 2.0f, 0.0f},
		{+kWidth / 2.0f, +kHeight / 2.0f, 0.0f},
		{-kWidth / 2.0f, +kHeight / 2.0f, 0.0f},
	};
	return Add(center, offsetTable[static_cast<uint32_t>(corner)]);
}

//判定結果を反映して移動
void Player::ResolveCollision(const CollisionMapInfo& info) {
	worldTransform_.translate = Add(info.movement_, worldTransform_.translate);
}

// 天井に当たっている場合
void Player::TopCollisionReaction(const CollisionMapInfo& info) {
	if (info.isHotTop_) {
		velocity_.y *= (1.0f - kAttenuationTop);
	}
}

void Player::WallCollisionReaction(const CollisionMapInfo& info) {
	if (info.isHitWall_) {
		velocity_.x *= (1.0f - kAttenuationWall);
	}
}

void Player::Update() {
	// 1.移動処理
	Move();

	// 2.移動量を加味した衝突判定
	CollisionMapInfo collisionMapinfo;      // 衝突情報初期化
	collisionMapinfo.movement_ = velocity_; // 移動量に速度をコピー
	isCollisionMap(collisionMapinfo);       // マップ衝突チェック

	// 3.判定結果を反映して移動
	ResolveCollision(collisionMapinfo);

	// 4.天井に当たっている場合
	TopCollisionReaction(collisionMapinfo);
	WallCollisionReaction(collisionMapinfo);

	// 着地フラグ
	CheckLanding(collisionMapinfo);


	// 旋回制御
	if (turnTimer_ > 0.0f) {

		{
			turnTimer_ -= 1.0f / 60.0f;
			// 左右の自キャラ角度テーブル
			float destinationRotationYTable[] = {
				std::numbers::pi_v<float> *-4.0f / 2.0f,
				std::numbers::pi_v<float> *2.0f / 2.0f,
			};
			// 状況に応じた角度を取得する
			float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
			// 自キャラの角度を設定する
			worldTransform_.rotate.y = destinationRotationY * (1.0f - turnTimer_);
		}
	}

	model_->Update(worldTransform_,camera_);

	if (Input::GetInstance()->IsTrigger(DIK_J)) { // 例: スペースキーで発射
		shootAnchor();
	}

	// アンカーの更新
	CollisionMapInfo info; // マップの衝突情報
	updateAnchor(info);

	isTeleported_ = false;

	if (Input::GetInstance()->IsTrigger(DIK_K)) {
		// アンカーが存在し、isStandByがtrueの場合
		if (anchor_ && anchor_->GetStandBy()) {
			// アンカーの位置を取得
			Vector3 anchorPos = anchor_->GetPosition();

			// テレポート先の座標を決定
			Vector3 teleportPosition = anchorPos;

			// プレイヤーの移動方向を判定
			if (anchor_->GetAngle()<2.0f) {
				// プレイヤーがアンカーの左にいる場合、テレポート後は右向き
				lrDirection_ = LRDirection::kRight;
				// 壁にめり込まないように、アンカーの位置からプレイヤーの幅の半分だけ右にずらす
				teleportPosition.x -= kWidth / 2.0f;
			} else {
				// プレイヤーがアンカーの右にいる場合、テレポート後は左向き
				lrDirection_ = LRDirection::kLeft;
				// 壁にめり込まないように、アンカーの位置からプレイヤーの幅の半分だけ左にずらす
				teleportPosition.x += kWidth / 2.0f;
			}

			// プレイヤーを計算された座標にテレポート
			worldTransform_.translate = teleportPosition;

			// アンカーを消去
			anchor_ = nullptr;
		}
	}
}

void Player::Draw() {
	// 自キャラの描画処理
	model_->Draw();
	if (anchor_ != nullptr) {
		anchor_->Draw();
	}
}

void Player::shootAnchor() {
	// アンカーがすでに存在する場合は何もしない
	if (anchor_ != nullptr) {
		return;
	}

	// プレイヤーが向いている方向を考慮してアンカーを生成
	Vector3 initialVelocity;
	if (lrDirection_ == LRDirection::kRight) {
		initialVelocity = { 0.3f, 0.0f,0.0f };
	} else {
		initialVelocity = { -0.3f, 0.0f,0.0f };
	}

	Vector3 spawnPos = { worldTransform_.translate.x, worldTransform_.translate.y,0.0f };
	anchor_ = std::make_unique<Anchor>(spawnPos, initialVelocity,mapChipField_);
}

void Player::updateAnchor(const CollisionMapInfo& info) {
	if (anchor_ != nullptr) {
		// アンカーを更新
		anchor_->Update(*camera_);
	}
}
