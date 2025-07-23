#include "EnemyBullet.h"
#include"../player/Player.h"

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity, Player& player) {
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

	worldMatrix_ = MakeIdentity4x4();

	player_ = &player;

}

EnemyBullet::~EnemyBullet() {

}

void EnemyBullet::Update(Camera* camera) {
	worldMatrix_ = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);

	Vector3 toPlayer = player_->GetWorldPosition() - GetWorldPosition();

	toPlayer = Normalize(toPlayer);
	velocity_ = Normalize(velocity_);

	velocity_ = Slerp(velocity_, toPlayer, 5.0f / 60.0f) * 1.0f;

	worldTransform_.rotate.y = std::atan2f(velocity_.x, velocity_.z);

	float velocityXZ = std::sqrtf(velocity_.x * velocity_.x + velocity_.z * velocity_.z);

	worldTransform_.rotate.x = std::atan2(velocity_.y, velocityXZ);

	worldMatrix_ = MakeIdentity4x4();

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

Vector3 EnemyBullet::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldMatrix_.m[3][0];
	worldPos.y = worldMatrix_.m[3][1];
	worldPos.z = worldMatrix_.m[3][2];

	return worldPos;
}
