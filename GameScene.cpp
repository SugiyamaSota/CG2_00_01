#include "GameScene.h"

void GameScene::Initialize(uint32_t clientWidth, uint32_t clientHeight) {
	// カメラ
	camera_ = new Camera();
	camera_->Initialize(clientWidth, clientHeight);

	// プレイヤー
	playerModel_ = new Model();
	playerModel_->LoadModel("cube");
	player_ = new Player();
	player_->Initialize(playerModel_);
}

GameScene::~GameScene() {
	delete player_;
	delete playerModel_;
	delete camera_;
}

void GameScene::Update() {
	// カメラ
	camera_->Update(Camera::CameraType::kDebug);

	// プレイヤー
	player_->Update(camera_);
}

void GameScene::Draw() {
	// プレイヤー
	player_->Draw();
}