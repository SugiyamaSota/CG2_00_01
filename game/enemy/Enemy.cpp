#include "Enemy.h"
#include <cmath>
#include <numbers>


void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {

	worldTransform_ = {
		{1,1,1},
		{0,0,0},
		position,
	};

	model_ = model;

	camera_ = camera;

	velocity_ = { -kWalkSpeed, 0, 0 };

	walkTimer_ = 0.0f;
}

void Enemy::Update() {

	// 移動処理
	//worldTransform_.translate = Add(worldTransform_.translate, velocity_);

	// 旋回制御
	TurningControl();

	// 縦方向の動き
	walkTimer_ += 1.0f / 60.0f;

	float param = std::sin(pi * 2 * walkTimer_ / kWalkTimer);
	float radian = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotate.z = radian * (pi / 180.0f);

	if (walkTimer_ >= kWalkTimer) {
		walkTimer_ = 0.0f;
	}

	// 行列の変換
	model_->Update(worldTransform_, camera_);
}

void Enemy::Draw() { model_->Draw(); }

void Enemy::TurningControl() {
	{
		float destinationRotationYTable[] = {
			std::numbers::pi_v<float> *-4.0f / 2.0f,
			std::numbers::pi_v<float> *2.0f / 2.0f,
		};
		// 状況に応じた角度を取得する
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];
		// 自キャラの角度を設定する
		worldTransform_.rotate.y = destinationRotationY;
	}
}

Vector3 Enemy::GetWorldPosition() {
	return worldTransform_.translate;
}


AABB Enemy::GetAABB() {
	AABB aabb;
	Vector3 worldPos = GetWorldPosition();
	aabb.min = { worldPos.x - kWidth_ / 2.0f, worldPos.y - kHeight_ / 2.0f, worldPos.z - kWidth_ / 2.0f };
	aabb.max = { worldPos.x + kWidth_ / 2.0f, worldPos.y + kHeight_ / 2.0f, worldPos.z + kWidth_ / 2.0f };
	return aabb;
}