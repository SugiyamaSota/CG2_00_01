#include "PlayerBullet.h"

void PlayerBullet::Initialize(Model* model, const Vector3& position) {
	// モデル
	assert(model);
	model_ = model;

	// ワールド変換
	worldTransform_ = InitializeWorldTransform();
	worldTransform_.translate = position;
}

PlayerBullet::~PlayerBullet() {

}

void PlayerBullet::Update(Camera* camera) {
	model_->Update(worldTransform_, camera, false);
}

void PlayerBullet::Draw() {
	model_->Draw();
}
