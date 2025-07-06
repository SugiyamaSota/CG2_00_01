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

	// 弾
	bulletModel_ = new Model();
	bulletModel_->LoadModel("cube");
}

Player::~Player() {
	delete bullet_;
	delete bulletModel_;
}

void Player::Update(Camera* camera) {
	// 移動
	Move();

	// 回転
	Rotate();

	// 攻撃
	Attack();

	if (bullet_) {
		bullet_->Update(camera);
	}

	model_->Update(worldTransform_, camera, false);
}

void Player::Move() {
	// キャラクターの移動ベクトル
	Vector3 move = { 0,0,0 };

	// キャラクターの移動の速さ
	const float kCharacterSpeed = 0.2f;

	// 押した方向で移動ベクトルを変更
	//左右
	if (Input::GetInstance()->IsPress(DIK_A)) {
		move.x -= kCharacterSpeed;
	} else if (Input::GetInstance()->IsPress(DIK_D)) {
		move.x += kCharacterSpeed;
	}
	// 上下
	if (Input::GetInstance()->IsPress(DIK_W)) {
		move.y += kCharacterSpeed;
	} else if (Input::GetInstance()->IsPress(DIK_S)) {
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
}

void Player::Rotate() {
	// 回転速度
	const float kRotSpeed = 0.05f;

	// 押した方向で旋回
	// 左右
	if (Input::GetInstance()->IsPress(DIK_LEFT)) {
		worldTransform_.rotate.y -= kRotSpeed;
	} else if (Input::GetInstance()->IsPress(DIK_RIGHT)) {
		worldTransform_.rotate.y += kRotSpeed;
	}
	// 上下
	if (Input::GetInstance()->IsPress(DIK_UP)) {
		worldTransform_.rotate.x -= kRotSpeed;
	} else if (Input::GetInstance()->IsPress(DIK_DOWN)) {
		worldTransform_.rotate.x += kRotSpeed;
	}
}

void Player::Attack() {
	if (Input::GetInstance()->IsTrigger(DIK_J)) {
		PlayerBullet* newBullet = new PlayerBullet();
		newBullet->Initialize(bulletModel_, worldTransform_.translate);

		// 弾を登録する
		bullet_ = newBullet;
	}
}

void Player::Draw() {
	// 弾
	if (bullet_) {
		bullet_->Draw();
	}

	// プレイヤー
	model_->Draw();


	ImGui::Begin("pos");
	ImGui::DragFloat3("position", &worldTransform_.translate.x);
	ImGui::End();
}
