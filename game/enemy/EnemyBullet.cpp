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

	worldTransform_.rotate.y= std::atan2f(velocity_.x, velocity_.z);

	float velocityXZ = std::sqrtf(velocity_.x * velocity_.x + velocity_.z * velocity_.z);

	worldTransform_.rotate.x = std::atan2(velocity_.y, velocityXZ);

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
