#include "GameScene.h"
#include"../others/Collision.h"
#include"../others/Data.h"

void GameScene::Initialize() {
	// カメラ
	camera_.Initialize(1280, 720);

	// マップチップフィールド
	mapChipField_ = std::make_unique<MapChipField>();
	mapChipField_->LoadmapChipCsv("resources/maps/tutrial.csv");

	// プレイヤー
	model_ = std::make_unique<Model>();
	model_->LoadModel("player");
	player_ = std::make_unique<Player>();
	Vector3 playerPosition = mapChipField_->GetMapChipPositionByIndex(2, 1);
	player_->Initialize(model_.get(), &camera_, playerPosition);
	player_->SetMapChipField(mapChipField_.get());

	// カメラのターゲット座標をプレイヤーの初期座標に設定
	cameraTarget_ = playerPosition;

	// 敵
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kEnemy) {
				// 新しいモデルを生成
				enemyModels_.push_back(std::make_unique<Model>());
				enemyModels_.back()->LoadModel("cube");

				// 新しい敵を生成
				enemies_.push_back(std::make_unique<Enemy>());
				// 敵の位置を縦に並べる
				Vector3 enemyPosition = mapChipField_->GetMapChipPositionByIndex(j,i);
				// 生成したモデルを敵に渡して初期化
				enemies_.back()->Initialize(enemyModels_.back().get(), &camera_, enemyPosition);
			}
		}
	}

	// ブロック
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			blockModel_[i][j] = new Model();
			blockModel_[i][j]->LoadModel("cube");
			blockWorldTransform_[i][j].rotate = { 0,0,0 };
			blockWorldTransform_[i][j].scale = { 1,1,1 };
			blockWorldTransform_[i][j].translate = { 0,0,0 };
		}
	}

	// ゴール
	goalModel_ = new Model();
	goalModel_->LoadModel("goal");
	goalWorldTransform_ = InitializeWorldTransform();
	isGoal_ = false;

	GenerateBlocksAndGoal();

	// 天球
	skydomeModel_ = std::make_unique<Model>();
	skydomeModel_->LoadModel("debugSkydome");
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get(), &camera_);
}

void GameScene::Update() {
	CheckGoal();
	// 当たり判定
	CheckAllCollisions();

	// カメラのターゲットを線形補間
	cameraTarget_ = Lerp(cameraTarget_, player_->GetPosition(), kCameraLerpRate);
	camera_.SetTarget(cameraTarget_);

	// カメラ
	camera_.Update(Camera::CameraType::kNormal);

	// 天球の更新
	skydome_->Update();

	// プレイヤー
	player_->Update();

	// 敵
	// リスト内のすべての敵を更新
	for (const auto& enemy : enemies_) {
		enemy->Update();
	}

	// ブロック
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				blockModel_[i][j]->Update(blockWorldTransform_[i][j], &camera_);
			}
		}
	}

	goalModel_->Update(goalWorldTransform_,&camera_);

	// Lキーが押されたら、ロックオン中の敵をすべて削除する
	if (Input::GetInstance()->IsTrigger(DIK_L)) {

		for (Enemy* enemy : lockedOnEnemies_) {
			enemy->SetIsDead(true); // 敵の死亡フラグを立てる
		}
		lockedOnEnemies_.clear(); // ロックオンリストをクリア
	}

	// 敵を死亡状態のものを削除
	enemies_.remove_if([&](const std::unique_ptr<Enemy>& enemy) {
		if (enemy->GetIsDead()) {
			lockedOnEnemies_.remove(enemy.get());
			return true;
		}
		return false;
		});
}

void GameScene::Draw() {
	// 天球の描画
	skydome_->Draw();

	// 自キャラの描画
	player_->Draw();

	// 敵
	// リスト内のすべての敵を描画
	for (const auto& enemy : enemies_) {
		enemy->Draw();
	}

	// ブロックの描画
	for (uint32_t i = 0; i < kNumBlockVirtical; ++i) {
		for (uint32_t j = 0; j < kNumBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				blockModel_[i][j]->Draw();
			}
		}
	}

	goalModel_->Draw();
}

void GameScene::GenerateBlocksAndGoal() {
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical();
	uint32_t numBlockHorizontal = mapChipField_->GetNumBlockHorizontal();

	const float kBlockWidth = 2.0f;
	const float kBlockHeight = 2.0f;

	for (uint32_t i = 0; i < numBlockVirtical; ++i) {
		for (uint32_t j = 0; j < numBlockHorizontal; ++j) {
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kBlock) {
				blockWorldTransform_[i][j].rotate = { 0,0,0 };
				blockWorldTransform_[i][j].scale = { 1,1,1 };
				blockWorldTransform_[i][j].translate = mapChipField_->GetMapChipPositionByIndex(j, i);
			}else 
			if (mapChipField_->GetMapChipTypeByIndex(j, i) == MapChipType::kGoal) {
				goalWorldTransform_.rotate = { 0,0,0 };
				goalWorldTransform_.scale = { 1,1,1 };
				goalWorldTransform_.translate = mapChipField_->GetMapChipPositionByIndex(j, i);
			}
		}
	}
}

void GameScene::CheckAllCollisions() {
	// AABB変数の宣言
	AABB playerAABB, enemyAABB, anchorAABB;

	// プレイヤーのAABBを取得
	playerAABB = player_->GetAABB();

	// リスト内の各敵と当たり判定
	for (const auto& enemy : enemies_) {
		// 敵のAABBを取得
		enemyAABB = enemy->GetAABB();

		// プレイヤーと敵の当たり判定
		if (IsCollision(playerAABB, enemyAABB)) {
			player_->OnCollision(enemy.get());
		}

		// アンカーと敵の当たり判定
		if (player_->HasAnchor()) {
			anchorAABB = player_->GetAnchor().GetAABB();
			if (IsCollision(anchorAABB, enemyAABB)) {
				player_->GetAnchor().OnCollision();

				// ロックオン処理
				// この敵がまだロックオンされていない場合のみ処理を行う
				if (!enemy->GetIsLockedOn()) {
					// ロックオンされた敵の数が上限に達しているかチェック
					if (lockedOnEnemies_.size() >= kMaxLockedOnEnemies) {
						// 一番古いロックオンを解除
						Enemy* oldestEnemy = lockedOnEnemies_.front();
						oldestEnemy->SetIsLockedOn(false);
						lockedOnEnemies_.pop_front();
					}

					// 新しい敵をロックオンリストに追加
					enemy->SetIsLockedOn(true);
					lockedOnEnemies_.push_back(enemy.get());
				}
			}
		}
	}
}

void GameScene::CheckGoal() {
	IndexSet playerIndexSet = mapChipField_->GetMapChipIndexSetByPosition(player_->GetPosition());
	MapChipType playerMapChip = mapChipField_->GetMapChipTypeByIndex(playerIndexSet.xIndex, playerIndexSet.yIndex);

	if (playerMapChip == MapChipType::kGoal) {
		isGoal_ = true;
	}
}