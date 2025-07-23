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
	Vector3 enemyStartPosition = { 20,5,120 };
	enemy_->Initialize(enemyModel_, enemyStartPosition);
	enemy_->SetPlayer(player_);
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
		} else
			if (cameraType_ == Camera::CameraType::kDebug) {
				cameraType_ = Camera::CameraType::kNormal;
			}
	}
#endif
	CheckAllCollisions();

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

void GameScene::CheckAllCollisions() {
	Vector3 posA, posB;

	const std::list<PlayerBullet*>playerBullets = player_->GetBullets();
	const std::list<EnemyBullet*>enemyBullets = enemy_->GetBullets();

#pragma region 自キャラと敵の弾
	posA = player_->GetWorldPosition();

	for (EnemyBullet* bullet : enemyBullets) {
		// 敵の弾の座標
		posB = bullet->GetWorldPosition();

		// 座標の差分
		Vector3 distance = posA - posB;
		float length = Length(distance);

		if (player_->GetRadius() + bullet->GetRadius() > length) {
			player_->OnCollision();
			bullet->OnCollision();
		}
	}
#pragma endregion

#pragma region 自分の弾と敵キャラ
	posA = enemy_->GetWorldPosition();

	for (PlayerBullet* bullet : playerBullets) {
		// 敵の弾の座標
		posB = bullet->GetWorldPosition();

		// 座標の差分
		Vector3 distance = posA - posB;
		float length = Length(distance);

		if (player_->GetRadius() + bullet->GetRadius() > length) {
			enemy_->OnCollision();
			bullet->OnCollision();
		}
	}
#pragma endregion


#pragma region 自分の弾と敵の弾
	for (PlayerBullet* playerBullet : playerBullets) {
		// 敵の弾の座標
		posA = playerBullet->GetWorldPosition();

		for (EnemyBullet* enemyBullet : enemyBullets) {
			// 敵の弾の座標
			posB = enemyBullet->GetWorldPosition();

			// 座標の差分
			Vector3 distance = posA - posB;
			float length = Length(distance);

			if (playerBullet->GetRadius() + enemyBullet->GetRadius() > length) {
				playerBullet->OnCollision();
				enemyBullet->OnCollision();
			}
		}
	}
#pragma endregion

}