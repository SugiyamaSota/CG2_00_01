#include "PlayerBullet.h"

// スケール引数を追加
void PlayerBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity, const Vector3& scale) { //
	// モデル
	assert(model);
	model_ = model;

	model_->SetColor({ 0,0,0,1 });

	// ワールド変換
	worldTransform_ = InitializeWorldTransform();
	worldTransform_.scale = scale; // 引数で受け取ったスケールを設定
	worldTransform_.translate = position;

	velocity_ = velocity;

	worldTransform_.rotate.y = std::atan2f(velocity_.x, velocity_.z);

	float velocityXZ = std::sqrtf(velocity_.x * velocity_.x + velocity_.z * velocity_.z);

	worldTransform_.rotate.x = std::atan2(velocity_.y, velocityXZ);

	SetCollisionAttibute(kCollisionAttibutePlayer);
	SetCollisionMask(kCollisionAttibuteEnemy);

}

PlayerBullet::~PlayerBullet() {

}

void PlayerBullet::Update(Camera* camera) {
	// 一定時間でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	worldTransform_.translate += velocity_;

	worldTransform_.worldMatrix = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
	if (worldTransform_.parent) {
		worldTransform_.parent->worldMatrix = MakeAffineMatrix(worldTransform_.parent->scale, worldTransform_.parent->rotate, worldTransform_.parent->translate);
		worldTransform_.worldMatrix = Multiply(worldTransform_.worldMatrix, worldTransform_.parent->worldMatrix);
	}

	model_->Update(worldTransform_, camera, false);
}

void PlayerBullet::Draw() {
	model_->Draw();
}

void PlayerBullet::OnCollision() {
	// デスフラグをたてる
	isDead_ = true;
}

Vector3 PlayerBullet::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.worldMatrix.m[3][0];
	worldPos.y = worldTransform_.worldMatrix.m[3][1];
	worldPos.z = worldTransform_.worldMatrix.m[3][2];

	return worldPos;
}