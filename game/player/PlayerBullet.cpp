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

}

PlayerBullet::~PlayerBullet() {

}

void PlayerBullet::Update(Camera* camera) {
	// 一定時間でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	worldTransform_.translate += velocity_;

	model_->Update(worldTransform_, camera, false);
}

void PlayerBullet::Draw() {
	model_->Draw();
}
