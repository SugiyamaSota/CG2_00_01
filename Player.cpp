#include "Player.h"
#include<cassert>

void Player::Initialize(Model* model) {
	// モデル
	assert(model);
	model_ = new Model();
	model_ = model;

	// ワールド変換
	worldTransform_ = InitializeWorldTransform();
}

Player::~Player() {
}

void Player::Update(Camera* camera) {
	model_->Update(worldTransform_, camera, false);
}

void Player::Draw() {
	model_->Draw();
}
