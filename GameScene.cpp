#include "GameScene.h"


void GameScene::Initialize(InputKey* key) {
	//モデルの生成
	model_ = new Model();
	model_->LoadModel("Player");

	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			blockModel_[i][j] = new Model();
			blockModel_[i][j]->LoadModel("cube");
			blockWorldTransform_[i][j].rotate = { 0,0,0 };
			blockWorldTransform_[i][j].scale = { 1,1,1 };
			blockWorldTransform_[i][j].translate.z = 0.0f;
			blockWorldTransform_[i][j].translate.x = kBlockWidth * j;
			blockWorldTransform_[i][j].translate.y = kBlockHeight * i;
		}
	}

	//カメラの初期化
	camera_.Initialize(key);
	//自キャラの生成と初期化
	player_ = new Player();
	player_->Initialize(model_, &camera_);
	
	skydomeModel_ = new Model();
	skydomeModel_->LoadModel("skydome");
	skydome_ = new Skydome();
	skydome_->Initialize(skydomeModel_, &camera_);
}

GameScene::~GameScene() {
	delete model_;
	delete player_;
	delete skydome_;
	delete skydomeModel_;

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			delete blockModel_[i][j];
		}
	}
	
}

void GameScene::Update() {
	camera_.Update();
	// 自キャラの更新
	player_->Update();
	// ブロックの更新
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			blockModel_[i][j]->Update(blockWorldTransform_[i][j], &camera_);
		}
	}
	// 天球の更新
	skydome_->Update();
}

void GameScene::Draw() {
	// 自キャラの描画
	player_->Draw();

	// ブロックの描画
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			blockModel_[i][j]->Draw();
		}
	}

	// 天球の描画
	skydome_->Draw();
}