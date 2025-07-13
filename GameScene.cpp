#include "GameScene.h"


void GameScene::Initialize() {
	// カメラの初期化
	camera_.Initialize();
	camera_.Update(false);

	// マップチップフィールド
	mapChipField_ = new MapChipField;
	mapChipField_->LoadmapChipCsv("resources/blocks.csv");

	// 自キャラの生成と初期化
	model_ = new Model();
	model_->LoadModel("player");
	playerAttackModel_ = new Model();
	playerAttackModel_->LoadModel("attackEffect");
	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 8);
	player_->Initialize(model_, playerAttackModel_,&camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);
	player_->Update();

	///// 敵に関する初期化 /////
	enemyModel_ = new Model();
	enemyModel_->LoadModel("enemy");
	Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(6, 6);
	enemy_ = new Enemy;
	enemy_->Initialize(enemyModel_, &camera_, enemyPosition);
	enemy_->Update();

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
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				blockModel_[i][j]->Update(blockWorldTransform_[i][j], { 1,1,1,1 }, &camera_);
			}
		}
	}

	// 天球
	skydomeModel_ = new Model();
	skydomeModel_->LoadModel("skydome");
	skydome_ = new Skydome();
	skydome_->Initialize(skydomeModel_, &camera_);
	skydome_->Update();

	phase_ = Phase::kFadeIn;

	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);

	
}

GameScene::~GameScene() {
	delete model_;
	delete player_;
	delete playerAttackModel_;
	delete deathParticles_;
	delete enemy_;
	delete enemyModel_;
	delete skydome_;
	delete skydomeModel_;
	delete mapChipField_;
	delete fade_;

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			delete blockModel_[i][j];
		}
	}

}

void GameScene::Update() {
	switch (phase_) {
	case Phase::kFadeIn:
		camera_.Update(isDebugCamera);
		fade_->Update();
		// ブロック
		for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
			for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
				if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
					blockModel_[i][j]->Update(blockWorldTransform_[i][j], { 1,1,1,1 }, &camera_);
				}
			}
		}
		break;
	case Phase::kPlay:
		// カメラ
		if (InputKey::GetInstance()->IsTrigger(DIK_SPACE)) {
			if (isDebugCamera) {
				isDebugCamera = false;
				camera_.ResetPosition();
				camera_.ResetRotation();
			} else {
				isDebugCamera = true;
				camera_.ResetPosition();
				camera_.ResetRotation();
			}
		}
		if (!isDebugCamera) {
			camera_.SetCameraTranslate(player_->GetPosition());
		}
		camera_.Update(isDebugCamera);
		// 天球
		skydome_->Update();
		// プレイヤー
		player_->Update();
		// 敵
		enemy_->Update();
		// ブロック
		for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
			for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
				if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
					blockModel_[i][j]->Update(blockWorldTransform_[i][j], { 1,1,1,1 }, &camera_);
				}
			}
		}
		CheckAllCollisions();
		break;
	case Phase::kDeath:
		camera_.Update(isDebugCamera);
		// 天球
		skydome_->Update();
		// 敵
		enemy_->Update();
		// デスパーティクル
		if (deathParticlesExistFlag) {
			deathParticles_->Update();
		};
		// ブロック
		for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
			for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
				if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
					blockModel_[i][j]->Update(blockWorldTransform_[i][j], { 1,1,1,1 }, &camera_);
				}
			}
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		break;
	}
	ChangePhase();
}

void GameScene::Draw() {
	switch (phase_) {
	case Phase::kFadeIn:
		// 自キャラ
		player_->Draw();
		// 敵
		enemy_->Draw();
		// ブロック
		for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
			for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
				if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
					blockModel_[i][j]->Draw();
				}
			}
		}
		// 天球
		skydome_->Draw();
		fade_->Draw();
		break;
	case Phase::kPlay:
		// 自キャラ
		player_->Draw();
		// 敵
		enemy_->Draw();
		// ブロック
		for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
			for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
				if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
					blockModel_[i][j]->Draw();
				}
			}
		}
		// 天球
		skydome_->Draw();
		break;
	case Phase::kDeath:
		// 敵
		enemy_->Draw();
		// ブロック
		for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
			for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
				if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
					blockModel_[i][j]->Draw();
				}
			}
		}
		// 天球
		skydome_->Draw();
		// デスパーティクル
		if (deathParticlesExistFlag) {
			deathParticles_->Draw();
		}
		break;
	case Phase::kFadeOut:
		// 敵
		enemy_->Draw();
		// ブロック
		for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
			for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
				if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
					blockModel_[i][j]->Draw();
				}
			}
		}
		// 天球
		skydome_->Draw();
		fade_->Draw();
		break;
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
	// 判定対象1と2の座標
	AABB aabb1, aabb2;
	// 自キャラの座標
	aabb1 = player_->GetAABB();

	// 自キャラと敵すべての当たり判定
	aabb2 = enemy_->GetAABB();

	if (IsCollision(aabb1, aabb2)) {
		player_->OnCollision(enemy_);
	}
}

void GameScene::ChangePhase() {
	switch (phase_) {
	case Phase::kFadeIn:
		if (fade_->IsFinished()) {
			phase_ = Phase::kPlay;
		}
		break;
	case Phase::kPlay:
		if (player_->IsDead()) {
			const Vector3& deathParticlePosition = player_->GetWorldPosition();
			deathParticles_ = new DeathParticles();
			deathParticles_->Initialize("Player", &camera_, deathParticlePosition);
			deathParticles_->Update();
			phase_ = Phase::kDeath;
		}
		break;
	case Phase::kDeath:
		if (deathParticles_ && deathParticles_->IsFinished()) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1);
		}
		break;
	case Phase::kFadeOut:
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
}