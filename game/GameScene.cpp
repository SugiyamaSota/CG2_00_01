#include "GameScene.h"

void GameScene::Initialize(uint32_t clientWidth, uint32_t clientHeight) {
	// カメラ
	camera_ = new Camera();
	camera_->Initialize(clientWidth, clientHeight);
	cameraType_ = Camera::CameraType::kNormal;

	//
	railCameraController_ = new RailCameraController();
	railCameraController_->Initialize({ 0,0,-50 }, 0.0f);

	// プレイヤー
	playerModel_ = new Model();
	playerModel_->LoadModel("monkey");
	player_ = new Player();
	Vector3 playerPosition = { 0,0,50 };
	player_->Initialize(playerModel_, playerPosition);
	player_->SetParent(&railCameraController_->GetWorldTransform());

	// 敵 (複数対応)
	// 敵の生成を関数にまとめる
	SpawnEnemy({ 20, 5, 120 }, "axis"); // 最初の敵
	SpawnEnemy({ -20, 5, 100 }, "monkey"); // 2番目の敵
	SpawnEnemy({ 0, 10, 80 }, "cube"); // 3番目の敵

	// 天球
	skydomeModel_ = new Model();
	skydomeModel_->LoadModel("debugSkydome");
	skydome_ = new Skydome();
	skydome_->Initialize(skydomeModel_, camera_);

	// 衝突マネージャー
	collisionManager_ = new CollisionManager;
	collisionManager_->Initialize(player_, &enemies_); // 敵のリストのポインタを渡す

	// ベジェ曲線が複数の線分で構成されるため、十分な頂点数を確保
	lineRenderer.Initialize(4096);
}

GameScene::~GameScene() {
	// 敵の弾丸とモデルのクリーンアップ
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}
	enemyBullets_.clear();
	for (Model* model : enemyBulletModels_) {
		delete model;
	}
	enemyBulletModels_.clear();

	// 敵リストのクリーンアップ
	for (Enemy* enemy : enemies_) {
		delete enemy;
	}
	enemies_.clear();

	// 各敵に割り当てられたモデルのクリーンアップを追加
	for (Model* model : enemyModels_) {
		delete model;
	}
	enemyModels_.clear();

	delete collisionManager_;
	delete skydome_;
	delete skydomeModel_;
	// delete enemyModel_; // <-- この行は削除
	// delete enemy_; // <-- この行は削除 (敵がリストになったため)
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
		} else
			if (cameraType_ == Camera::CameraType::kDebug) {
				cameraType_ = Camera::CameraType::kNormal;
			}
	}
#endif
	// Update Collision Manager with enemy bullets and enemies
	collisionManager_->Update(enemyBullets_, player_->GetBullets(), enemies_); // 敵のリストも渡す

	railCameraController_->Update();
	camera_->SetViewMatrix(railCameraController_->GetViewMatrix());
	camera_->Update(cameraType_);

	// プレイヤー
	player_->Update(camera_);

	// 敵リストの更新
	enemies_.remove_if([](Enemy* enemy) {
		// 必要に応じて敵が削除される条件を追加
		// 例: if (enemy->IsDead()) { delete enemy; return true; }
		return false; // 今回は常にfalseで削除しない
		});

	for (Enemy* enemy : enemies_) {
		enemy->Update(camera_);
	}

	// Update enemy bullets
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});

	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update(camera_);
	}

	// 天球
	skydome_->Update();

	lineRenderer.Clear();

	lineRenderer.AddBezierPath(bezierPathPoints, 30, Color::Green, *camera_);
}

void GameScene::Draw() {
	// プレイヤー
	player_->Draw();

	// 敵リストの描画
	for (Enemy* enemy : enemies_) {
		enemy->Draw();
	}

	// Draw enemy bullets
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw();
	}

	// 天球
	skydome_->Draw();

	lineRenderer.Draw();
}

void GameScene::AddEnemyBullet(EnemyBullet* bullet, Model* model) {
	enemyBullets_.push_back(bullet);
	enemyBulletModels_.push_back(model); // Store model for cleanup
}

void GameScene::SpawnEnemy(const Vector3& position, const std::string& modelPath) {
	Model* newEnemyModel = new Model();
	newEnemyModel->LoadModel(modelPath);

	Enemy* newEnemy = new Enemy();
	newEnemy->Initialize(newEnemyModel, position);
	newEnemy->SetPlayer(player_);
	newEnemy->SetGameScene(this);

	enemies_.push_back(newEnemy);
	enemyModels_.push_back(newEnemyModel);
}