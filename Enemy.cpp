#include "Enemy.h"
#include <cmath>
#include <numbers>
#include"Player.h"

void Enemy::Initialize(Model* model, DebugCamera* camera, const Vector3& position) {

	worldTransform_ = {
		{1,1,1},
		{0,0,0},
		position,
	};

	model_ = model;

	camera_ = camera;

	velocity_ = { -kWalkSpeed, 0, 0 };

	walkTimer_ = 0.0f;

	isDead_ = false;

	deathParameter_ = 0;

	isDeathMotion_ = false;
}

void Enemy::Update() {

	if (behaviorRequest_ != Behavior::kUnknown) {
		// ふるまいを変更する
		behavior_ = behaviorRequest_;

		// 各振る舞いごとの初期化
		switch (behavior_) {
		case Behavior::kWalk:
			
			break;
		case Behavior::kDeath:
			velocity_.y = deathjump_;
			isDeathMotion_ = true;
			break;
		}

		// ふるまいのリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	switch (behavior_) {
	case Behavior::kWalk:
		BehaviorWalkUpdate();
		break;
	case Behavior::kDeath:
		BehaviorDeathUpdate();
		break;
	}

	// 行列の変換
	model_->Update(worldTransform_, { 1,1,1,1 }, camera_);
}

void Enemy::BehaviorWalkUpdate() {
	// 移動処理
	worldTransform_.translate = Add(worldTransform_.translate, velocity_);

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

	
}

void Enemy::BehaviorDeathUpdate() {
	float t= static_cast<float>(deathParameter_) / deathTime_;
	worldTransform_.rotate.y++;

	// 死んだ瞬間に飛ばせて地底まで落下させたい
	velocity_.x = kWalkSpeed;
	velocity_.y -= 0.0098f;

	// 移動処理
	worldTransform_.translate = Add(worldTransform_.translate, velocity_);

	if (deathParameter_ >= deathTime_) {
		isDead_ = true;
	}

	deathParameter_++;
}

void Enemy::Draw() { model_->Draw(); }

void Enemy::TurningControl() {
	{
		float destinationRotationYTable[] = {
			std::numbers::pi_v<float> * -2.0f / 2.0f,
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

void Enemy::OnCollision(const Player* player) {
	(void)player;

	if (behavior_ == Behavior::kDeath) {
		return;
	}

	if (player->IsAttack()) {
		behaviorRequest_ = Behavior::kDeath;
		
	}
}