#include "GameScene.h"


void GameScene::Initialize(InputKey* key) {
	// カメラの初期化
	camera_.Initialize(key);

	// 自キャラの生成と初期化
	model_ = new Model();
	model_->LoadModel("Player");
	player_ = new Player();
	player_->Initialize(model_, &camera_);
	
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
				blockModel_[i][j] = new Model();
				blockModel_[i][j]->LoadModel("cube");
				blockWorldTransform_[i][j].rotate = { 0,0,0 };
				blockWorldTransform_[i][j].scale = { 1,1,1 };
				blockWorldTransform_[i][j].translate = { 0,0,0 };
		}
	}
	// マップチップフィールド
	mapChipField_ = new MapChipField;
	mapChipField_->LoadmapChipCsv("resources/blocks.csv");
	GenerateBlocks();

	// 天球
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
	delete mapChipField_;

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
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				blockModel_[i][j]->Update(blockWorldTransform_[i][j], &camera_);
			}
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
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				blockModel_[i][j]->Draw();
			}
		}
	}

	// 天球の描画
	skydome_->Draw();
}

void GameScene::GenerateBlocks() {
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				blockWorldTransform_[i][j].rotate = { 0,0,0 };
				blockWorldTransform_[i][j].scale = { 1,1,1 };
				blockWorldTransform_[i][j].translate = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}