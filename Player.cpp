#include"Player.h"


void Player::Initialize(Model* model, DebugCamera* camera) {
	assert(model);
	model_ = model;

	worldTransform_ = {
		{1,1,1},
		{0,0,0},
		{0,0,0},
	};

	camera_ = camera;
}

void Player::Update() {
	// 行列を定数バッファに転送
	model_->Update(worldTransform_, camera_);
}

void Player::Draw() {
	// 自キャラの描画処理
	model_->Draw();
}