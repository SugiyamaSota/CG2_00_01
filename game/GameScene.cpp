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
	std::list<Collider*>colliders_;

	colliders_.push_back(player_);
	colliders_.push_back(enemy_);

	const std::list<PlayerBullet*>playerBullets = player_->GetBullets();
	const std::list<EnemyBullet*>enemyBullets = enemy_->GetBullets();

	for (PlayerBullet* bullet : playerBullets) {
		colliders_.push_back(bullet);
	}

	for (EnemyBullet* bullet : enemyBullets) {
		colliders_.push_back(bullet);
	}

	std::list<Collider*>::iterator itrA = colliders_.begin();

	for (; itrA != colliders_.end(); ++itrA) {
		Collider* colliderA = *itrA;
		std::list<Collider*>::iterator itrB = itrA;
		itrB++;
		for (; itrB != colliders_.end(); ++itrB) {
			Collider* colliderB = *itrB;

			CheckCollisionPair(colliderA, colliderB);
		}
	}
}

void GameScene::CheckCollisionPair(Collider* colliderA, Collider* colliderB) {
	// フィルター
	if (
		(colliderA->GetCollisionAttibute() ^ colliderB->GetCollisionMask() ||
			colliderB->GetCollisionAttibute() ^ colliderA->GetCollisionMask())
		) {
		return;
	}

		Vector3 posA, posB;
	posA = colliderA->GetWorldPosition();
	posB = colliderB->GetWorldPosition();
	// 座標の差分
	Vector3 distance = posA - posB;
	float length = Length(distance);
	if (colliderA->GetRadius() + colliderB->GetRadius() > length) {
		colliderA->OnCollision();
		colliderB->OnCollision();
	}
}