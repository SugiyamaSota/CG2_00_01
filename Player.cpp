#include "Player.h"
#include<cassert>
#include<algorithm>

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
	// キャラクターの移動ベクトル
	Vector3 move = { 0,0,0 };

	// キャラクターの移動の速さ
	const float kCharacterSpeed = 0.2f;

	// 押した方向で移動ベクトルを変更
	//左右
	if (Input::GetInstance()->IsPress(DIK_LEFT)) {
		move.x -= kCharacterSpeed;
	} else if (Input::GetInstance()->IsPress(DIK_RIGHT)) {
		move.x += kCharacterSpeed;
	}
	// 上下
	if (Input::GetInstance()->IsPress(DIK_UP)) {
		move.y += kCharacterSpeed;
	} else if (Input::GetInstance()->IsPress(DIK_DOWN)) {
		move.y -= kCharacterSpeed;
	}

	// 座標移動
	worldTransform_.translate += move;

	// 座標制限
	const float kMoveLimitX = 19.0f;
	const float kMoveLimitY = 10.0f;

	worldTransform_.translate.x = max(worldTransform_.translate.x, -kMoveLimitX);
	worldTransform_.translate.x = min(worldTransform_.translate.x, kMoveLimitX);
	worldTransform_.translate.y = max(worldTransform_.translate.y, -kMoveLimitY);
	worldTransform_.translate.y = min(worldTransform_.translate.y, kMoveLimitY);

	model_->Update(worldTransform_, camera, false);
}

void Player::Draw() {
	model_->Draw();

	ImGui::Begin("pos");
	ImGui::DragFloat3("position", &worldTransform_.translate.x);
	ImGui::End();
}
