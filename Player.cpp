#define NOMINMAX
#include "Player.h"
#include <algorithm>
#include <numbers>

void Player::Initialize(Model* model, DebugCamera* camera,Vector3 position) {
	assert(model);
	model_ = model;

	worldTransform_ = {
		{1,1,1},
		{0,0,0},
		position,
	};

	camera_ = camera;
}

void Player::Update(InputKey* key) {
	// 移動入力
	// //接地状態
	if (onGround_) {
		// 左右移動操作
		if (key->IsPress(DIK_RIGHT) || key->IsPress(DIK_LEFT)) {
			// 左右加速
			Vector3 acceleration = {};
			if (key->IsPress(DIK_RIGHT)) {
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
			} else if (key->IsPress(DIK_LEFT)) {
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
		if (key->IsPress(DIK_UP)) {
			velocity_ = Add(velocity_, Vector3(0, kJumpAcceleration, 0));
		}
	} else {
		// 空中
		velocity_ = Add(velocity_, Vector3(0, -kGravityAcceleration, 0));
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	// 着地フラグ
	bool landing = false;

	if (velocity_.y < 0) {
		if (worldTransform_.translate.y <= 1.0f) {
			landing = true;
		}
	}

	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}
	} else {
		if (landing) {
			worldTransform_.translate.y = 1.0f;
			velocity_.x *= (1.0f - kAttenuation);
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}

	// 移動
	worldTransform_.translate = Add(worldTransform_.translate, velocity_);

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
	// 行列更新
	// 行列を定数バッファに転送
	model_->Update(worldTransform_, camera_);
}

void Player::Draw() {
	// 自キャラの描画処理
	model_->Draw();
}