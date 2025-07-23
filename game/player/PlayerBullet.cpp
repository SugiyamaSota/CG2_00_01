#include "PlayerBullet.h"

void PlayerBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity) {
	// モデル
	assert(model);
	model_ = model;

	model_->SetColor({ 0,0,0,1 });

	// ワールド変換
	worldTransform_ = InitializeWorldTransform();
	worldTransform_.translate = position;

	velocity_ = velocity;

	worldMatrix_ = MakeIdentity4x4();

}

PlayerBullet::~PlayerBullet() {

}

void PlayerBullet::Update(Camera* camera) {
	// 一定時間でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	worldTransform_.translate += velocity_;

	worldMatrix_ = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);

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

	worldPos.x = worldMatrix_.m[3][0];
	worldPos.y = worldMatrix_.m[3][1];
	worldPos.z = worldMatrix_.m[3][2];

	return worldPos;
}
