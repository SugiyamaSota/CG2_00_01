#include "GameScene.h"
#include"../others/Collision.h"

void GameScene::Initialize() {
	// カメラの初期化
	camera_.Initialize(1280, 720);

	// マップチップフィールド
	mapChipField_ = new MapChipField;
	mapChipField_->LoadmapChipCsv("resources/maps/tutrial.csv");

	// 自キャラの生成と初期化
	model_ = new Model();
	model_->LoadModel("player");
	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 1);
	player_->Initialize(model_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);

	///// 敵に関する初期化 /////
	enemyModel_ = new Model();
	enemyModel_->LoadModel("cube");
	Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(6, 6);
	enemy_ = new Enemy;
	enemy_->Initialize(enemyModel_, &camera_, enemyPosition);

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			blockModel_[i][j] = new Model();
			blockModel_[i][j]->LoadModel("cube");
			blockWorldTransform_[i][j].rotate = { 0,0,0 };
			blockWorldTransform_[i][j].scale = { 1,1,1 };
			blockWorldTransform_[i][j].translate = { 0,0,0 };
		}
	}
	GenerateBlocks();

	// 天球
	skydomeModel_ = new Model();
	skydomeModel_->LoadModel("debugSkydome");
	skydome_ = new Skydome();
	skydome_->Initialize(skydomeModel_, &camera_);
}

GameScene::~GameScene() {
	delete model_;
	delete player_;
	delete enemy_;
	delete enemyModel_;
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
	// 当たり判定
	CheckAllCollisions();

	// カメラ
	camera_.Update(Camera::CameraType::kNormal);
	camera_.SetTarget(player_->GetPosition());

	// 天球の更新
	skydome_->Update();

	// プレイヤー
	player_->Update();

	// 敵
	enemy_->Update();

	// ブロック
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				blockModel_[i][j]->Update(blockWorldTransform_[i][j], &camera_);
			}
		}
	}
}

void GameScene::Draw() {

	// 天球の描画
	skydome_->Draw();

	// 自キャラの描画
	player_->Draw();

	// 敵
	enemy_->Draw();

	// ブロックの描画
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				blockModel_[i][j]->Draw();
			}
		}
	}
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

void GameScene::CheckAllCollisions() {
	// AABB変数の宣言
	AABB player, enemy, anchor;

	// AABBの取得
	player = player_->GetAABB();
	enemy = enemy_->GetAABB();

	// プレイヤーと敵
	if (IsCollision(player, enemy)) {
		player_->OnCollision(enemy_);
	}

	// アンカーと敵
	if (player_->HasAnchor()) {
		anchor = player_->GetAnchor().GetAABB();
		if (IsCollision(anchor, enemy)) {
			player_->GetAnchor().OnCollision();
		}
	}
}