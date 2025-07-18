#include "GameScene.h"


void GameScene::Initialize() {
	// カメラの初期化
	camera_.Initialize();
	camera_.Update(false);

	// マップチップフィールド
	mapChipField_ = new MapChipField;
	mapChipField_->LoadmapChipCsv("resources/blocks.csv");

	// 自キャラの生成と初期化
	playerModel_ = new Model();
	playerModel_->LoadModel("Player");
	playerAttackModel_ = new Model();
	playerAttackModel_->LoadModel("attackEffect");
	player_ = new Player();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 8);
	player_->Initialize(playerModel_, playerAttackModel_, &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_);
	player_->Update();

	///// 敵に関する初期化 /////
	for (int i = 0; i < 3; ++i) {
		Model* newModel = new Model();
		newModel->LoadModel("enemy");

		enemyModels_.push_back(newModel);

		Enemy* newEnemy = new Enemy();
		Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(6 + i, 6 + i*2);
		newEnemy->Initialize(newModel, &camera_, enemyPosition);

		enemies_.push_back(newEnemy);
	}
	// 敵
	for (Enemy* enemy : enemies_) {
		enemy->Update();
	}



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

	// ヒットエフェクト
	HitEffect::SetCamera(&camera_);
}

GameScene::~GameScene() {
	// HitEffect インスタンスの解放
	for (HitEffect* hitEffect : hitEffects_) {
		delete hitEffect;
	}
	hitEffects_.clear();

	delete playerModel_;
	delete player_;
	delete playerAttackModel_;
	delete deathParticles_;

	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	for (Model* model : enemyModels_) {
		delete model;
	}
	enemyModels_.clear();

	delete skydome_;
	delete skydomeModel_;
	delete mapChipField_;
	delete fade_;

	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			delete blockModel_[i][j];
		}
	}

	for (Model* model : hitEffectCircleModel_) {
		delete model;
	}
	hitEffectCircleModel_.clear(); // リストもクリアする

	for (Model* model : hitEffectEllipseModel_) {
		delete model;
	}
	hitEffectEllipseModel_.clear(); // リストもクリアする
}

void GameScene::Update() {
	// 非フェードアウト時
	if (phase_ != Phase::kFadeOut) {
		// カメラの更新
		camera_.Update(isDebugCamera);

		// 敵の更新
		for (Enemy* enemy : enemies_) {
			enemy->Update();
		}
		// 敵のデスフラグ処理

		enemies_.remove_if([](Enemy* enemy) {
			if (enemy->IsDead()) {
				delete enemy;
				return true;
			}
			return false;
			});

		// ブロックの更新
		for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
			for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
				if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
					blockModel_[i][j]->Update(blockWorldTransform_[i][j], { 1,1,1,1 }, &camera_);
				}
			}
		}
	}

	// 非フェード時
	if (phase_ != Phase::kFadeIn && phase_ != Phase::kFadeOut) {
		// 天球の更新
		skydome_->Update();
	}

	/// --- 各フェーズ固有の更新処理 ---
	switch (phase_) {
	case Phase::kFadeIn:
		// フェードインの更新
		fade_->Update();
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
		// 非デバッグカメラ時のプレイヤー追従
		if (!isDebugCamera) {
			camera_.SetCameraTranslate(player_->GetPosition());
		}
		// プレイヤー
		player_->Update();
		// ヒットエフェクト
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Update();
		}
		// 当たり判定のチェック
		CheckAllCollisions();
		break;
	case Phase::kDeath:
		// デスパーティクル
		if (deathParticlesExistFlag) {
			deathParticles_->Update();
		};
		break;
	case Phase::kFadeOut:
		// フェードアウトの更新
		fade_->Update();
		break;
	}

	/// --- 共通の更新処理 ---
	// フェーズの変更処理
	ChangePhase();
}
void GameScene::Draw() {
	/// --- 共通の描画処理 ---
	// 敵
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}
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

	/// --- 各フェーズ固有の描画処理 ---
	switch (phase_) {
	case Phase::kFadeIn:
		// 自キャラ
		player_->Draw();
		// ヒットエフェクト
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Draw();
		}
		fade_->Draw();
		break;
	case Phase::kPlay:
		// 自キャラ
		player_->Draw();
		for (HitEffect* hitEffect : hitEffects_) {
			hitEffect->Draw();
		}
		break;
	case Phase::kDeath:
		// デスパーティクル
		if (deathParticlesExistFlag) {
			deathParticles_->Draw();
		}
		break;
	case Phase::kFadeOut:
		// フェード
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
	AABB aabb1; // aabb2 はループ内で代入されるため、ここで初期化する必要はありません。
	// 自キャラの座標
	aabb1 = player_->GetAABB();

	// 自キャラと敵すべての当たり判定
	for (Enemy* enemy : enemies_) {
		AABB aabb2 = enemy->GetAABB(); // ✨ 修正点: 敵のAABBをaabb2に代入します。

		if (IsCollision(aabb1, aabb2)) {
			player_->OnCollision(enemy);
			enemy->OnCollision(player_,this);
		}
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

const void GameScene::CreateEffect(Vector3 startPosition) {
	Model* newCircleModel = new Model();
	newCircleModel->LoadModel("hitEffect");
	hitEffectCircleModel_.push_back(newCircleModel);

	// 2つの異なる楕円モデルを作成
	Model* newEllipseModel1 = new Model();
	newEllipseModel1->LoadModel("hitEffect");
	hitEffectEllipseModel_.push_back(newEllipseModel1);

	Model* newEllipseModel2 = new Model();
	newEllipseModel2->LoadModel("hitEffect");
	hitEffectEllipseModel_.push_back(newEllipseModel2);

	// std::array を作成し、生成した2つのモデルポインタを格納
	std::array<Model*, 2> ellipseModelsArray = { newEllipseModel1, newEllipseModel2 };

	HitEffect* newHitEffect = new HitEffect();
	// Create 関数に std::array を渡す
	newHitEffect = HitEffect::Create(newCircleModel, ellipseModelsArray, startPosition);
	hitEffects_.push_back(newHitEffect); 
}