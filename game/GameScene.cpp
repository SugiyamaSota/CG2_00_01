#include "GameScene.h"

#include<cassert>
#include<fstream>

void GameScene::Initialize(uint32_t clientWidth, uint32_t clientHeight) {
	// カメラ
	camera_ = new Camera();
	camera_->Initialize(clientWidth, clientHeight);
	cameraType_ = Camera::CameraType::kNormal;

	//
	railCameraController_ = new RailCameraController();
	// ベジェ曲線パスをRailCameraControllerに渡して初期化
	railCameraController_->Initialize(&bezierPathPoints, 0.0005f, 5.0f, 1.0f, clientWidth, clientHeight);

	// プレイヤー
	playerModel_ = new Model();
	playerModel_->LoadModel("monkey");
	player_ = new Player();
	Vector3 playerPosition = { 0,0,50 };
	player_->Initialize(playerModel_, playerPosition);
	player_->SetParent(&railCameraController_->GetWorldTransform()); // RailCameraControllerのWorldTransformを親に設定

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

	LoadEnemyPopData();
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
	delete railCameraController_; // railCameraController_ のdeleteを追加
}

void GameScene::Update() {
	UpdateEnemyPopCommands();

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


	// RailCameraController を更新
	railCameraController_->Update(&lineRenderer);
	// RailCameraController から更新されたビュー行列をカメラに設定
	camera_->SetViewMatrix(railCameraController_->GetViewMatrix());
	camera_->Update(cameraType_); // Debugカメラとの切り替えは必要に応じて残す


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

	// ベジェ曲線パスの描画
	lineRenderer.AddBezierPath(bezierPathPoints, 30, Color::Green, *camera_);
}

void GameScene::Draw() {

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


	// プレイヤー
	player_->Draw();

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

void GameScene::LoadEnemyPopData() {
	/// ファイルを開く
	std::ifstream file;
	file.open("resources/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容をコピー
	enemyPopCommands << file.rdbuf();

	file.close();
}

void GameScene::UpdateEnemyPopCommands() {
	//
	if (isWaiting_) {
		waitTimer_--;
		if (waitTimer_ <= 0) {
			isWaiting_ = false;
		}
		return;
	}

	//
	std::string line;

	//
	while (getline(enemyPopCommands, line)) {
		//
		std::istringstream line_stream(line);

		std::string word;
		//
		getline(line_stream, word, ',');
		//
		if (word.find("//") == 0) {
			//
			continue;
		}
		//
		if (word.find("POP") == 0) {
			// x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());
			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());
			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 
			getline(line_stream, word);
			std::string modelName = word;
			

			SpawnEnemy(Vector3(x, y, z), modelName);
		} else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			//
			int32_t waitTime = atoi(word.c_str());

			//
			isWaiting_ = true;
			waitTimer_ = waitTime;

			//
			break;
		}
	}
}