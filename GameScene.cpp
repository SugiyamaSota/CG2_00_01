#include "GameScene.h"

void GameScene::Initialize(uint32_t clientWidth, uint32_t clientHeight) {
	// カメラ
	camera_ = new Camera();
	camera_->Initialize(clientWidth, clientHeight);
	cameraType_ = Camera::CameraType::kNormal;

	// プレイヤー
	playerModel_ = new Model();
	playerModel_->LoadModel("monkey");
	player_ = new Player();
	player_->Initialize(playerModel_);

	// 敵
	enemyModel_ = new Model();
	enemyModel_->LoadModel("axis");
	enemy_ = new Enemy();
	Vector3 enemyStartPosition = { 0,0,0 };
	enemy_->Initialize(enemyModel_, enemyStartPosition);
}

GameScene::~GameScene() {
	delete enemy_;
	delete enemyModel_;
	delete player_;
	delete playerModel_;
	delete camera_;
}

void GameScene::Update() {
	// カメラ
#ifdef _DEBUG
	if (Input::GetInstance()->IsTrigger(DIK_SPACE)) {
		if (cameraType_ == Camera::CameraType::kNormal) {
			cameraType_ = Camera::CameraType::kDebug;
		}else 
		if (cameraType_ == Camera::CameraType::kDebug) {
			cameraType_ = Camera::CameraType::kNormal;
		}
	}
#endif
	camera_->Update(cameraType_);

	// プレイヤー
	player_->Update(camera_);

	// 敵
	enemy_->Update(camera_);
}

void GameScene::Draw() {
	// プレイヤー
	player_->Draw();

	// 敵
	enemy_->Draw();
}