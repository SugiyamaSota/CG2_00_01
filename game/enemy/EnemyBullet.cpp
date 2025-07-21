#include "EnemyBullet.h"

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	// モデル
	assert(model);
	model_ = model;

	model_->SetColor({ 1,0,0,1 });

	// ワールド変換
	worldTransform_ = InitializeWorldTransform();
	worldTransform_.translate = position;
	worldTransform_.scale = { 0.5f,0.5f,3.0f };

	velocity_ = velocity;

	float pitch = 0.0f; // X軸周りの回転
	float yaw = 0.0f;   // Y軸周りの回転
	float roll = 0.0f;  // Z軸周りの回転 
		yaw = std::atan2f(velocity_.x, velocity_.z);

		float horizontalLength = std::sqrtf(velocity_.x * velocity_.x + velocity_.z * velocity_.z);

		if (horizontalLength < 0.0001f) {
			pitch = std::atan2f(velocity_.y, 0.0f);
		} else {
			pitch = std::atan2f(velocity_.y, horizontalLength);
		}

		roll = 0.0f;
	
	worldTransform_.rotate = { pitch, yaw, roll };
}

EnemyBullet::~EnemyBullet() {

}

void EnemyBullet::Update(Camera* camera) {
	// 一定時間でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	worldTransform_.translate += velocity_;

	model_->Update(worldTransform_, camera, false);
}

void EnemyBullet::Draw() {
	model_->Draw();
}
