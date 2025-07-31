#include "Player.h"
#include<cassert>
#include<algorithm>

void Player::Initialize(Model* model, Vector3 position) {
	// モデル
	assert(model);
	model_ = new Model();
	model_ = model;

	// ワールド変換
	worldTransform_ = InitializeWorldTransform();
	worldTransform_.translate = position;

	SetCollisionAttibute(kCollisionAttibutePlayer);
	SetCollisionMask(kCollisionAttibuteEnemy);

	// レティクル　
	worldTransform3DReticle_ = InitializeWorldTransform();
	reticleModel_ = new Model;
	reticleModel_->LoadModel("cube");
	reticleModel_->Initialize(worldTransform3DReticle_);

	sprite2DReticle_ = new Sprite;
	Vector3 spritePosition = { 640,320,0 };
	sprite2DReticle_->Initialize(spritePosition, Color::White, { 0.5f,0.5f,0.0f }, { 128.0f,128.0f }, "reticle.png");
}

Player::~Player() {
	for (PlayerBullet* bullet : bullets_) {
		delete bullet;
	}
	for (Model* model : bulletModel_) {
		delete model;
	}
	delete reticleModel_;
	delete sprite2DReticle_;
}

void Player::Update(Camera* camera) {
	// 移動
	Move();

	// 回転
	Rotate();

	// 攻撃
	Attack();

	// デスフラグが立った弾を削除
	bullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});

	for (PlayerBullet* bullet : bullets_) {
		bullet->Update(camera);
	}

	worldTransform_.worldMatrix = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
	if (worldTransform_.parent) {
		worldTransform_.parent->worldMatrix = MakeAffineMatrix(worldTransform_.parent->scale, worldTransform_.parent->rotate, worldTransform_.parent->translate);
		worldTransform_.worldMatrix = Multiply(worldTransform_.worldMatrix, worldTransform_.parent->worldMatrix);
	}

	model_->Update(worldTransform_, camera, false);

	// 3Dレティクル
	{
		// 自機からレティクルの距離
		const float kDistancePlayerTo3DReticle = 50.0f;
		// オフセット
		Vector3 offset = { 0,0,1.0f };

		offset = TransformNormal(offset, worldTransform_.worldMatrix);
		// 正規化
		offset = Normalize(offset) * kDistancePlayerTo3DReticle;
		//
		worldTransform3DReticle_.translate = GetWorldPosition() + offset;
		worldTransform3DReticle_.worldMatrix = MakeAffineMatrix(worldTransform3DReticle_.scale, worldTransform3DReticle_.rotate, worldTransform3DReticle_.translate);
		if (worldTransform3DReticle_.parent) {
			worldTransform3DReticle_.parent->worldMatrix = MakeAffineMatrix(worldTransform3DReticle_.parent->scale, worldTransform3DReticle_.parent->rotate, worldTransform3DReticle_.parent->translate);
			worldTransform3DReticle_.worldMatrix = Multiply(worldTransform3DReticle_.worldMatrix, worldTransform3DReticle_.parent->worldMatrix);
		}
		reticleModel_->Update(worldTransform3DReticle_, camera, false);
	}

	// 2Dレティクル
	{
		Vector3 positionReticle = GetReticleWorldPosition();

		// ビューポート
		Matrix4x4 viewportMat = MakeViewportMatrix(0, 0, 1280, 720, 0, 1);

		Matrix4x4 viewProjectionViewportMat = camera->GetViewProjectionMatrix() * viewportMat;
		//
		positionReticle = Transform(positionReticle, viewProjectionViewportMat);

		sprite2DReticle_->Update(positionReticle, Color::White);
	}

}

void Player::Move() {
	// キャラクターの移動ベクトル
	Vector3 move = { 0,0,0 };

	// キャラクターの移動の速さ
	const float kCharacterSpeed = 0.2f;

	// 押した方向で移動ベクトルを変更
	//左右
	if (Input::GetInstance()->IsPress(DIK_A)) {
		move.x -= kCharacterSpeed;
	} else if (Input::GetInstance()->IsPress(DIK_D)) {
		move.x += kCharacterSpeed;
	}
	// 上下
	if (Input::GetInstance()->IsPress(DIK_W)) {
		move.y += kCharacterSpeed;
	} else if (Input::GetInstance()->IsPress(DIK_S)) {
		move.y -= kCharacterSpeed;
	}

	// 座標移動
	worldTransform_.translate += move;

	// 座標制限
	const float kMoveLimitX = 19.0f;
	const float kMoveLimitY = 10.0f;

	worldTransform_.translate.x = max(worldTransform_.translate.x, -kMoveLimitX);
	worldTransform_.translate.x = min(worldTransform_.translate.x, kMoveLimitX);
	worldTransform_.translate.y = max(worldTransform_.translate.y, -kMoveLimitY);
	worldTransform_.translate.y = min(worldTransform_.translate.y, kMoveLimitY);
}

void Player::Rotate() {
	// 回転速度
	const float kRotSpeed = 0.05f;

	// 押した方向で旋回
	// 左右
	if (Input::GetInstance()->IsPress(DIK_LEFT)) {
		worldTransform_.rotate.y -= kRotSpeed;
	} else if (Input::GetInstance()->IsPress(DIK_RIGHT)) {
		worldTransform_.rotate.y += kRotSpeed;
	}
	// 上下
	if (Input::GetInstance()->IsPress(DIK_UP)) {
		worldTransform_.rotate.x -= kRotSpeed;
	} else if (Input::GetInstance()->IsPress(DIK_DOWN)) {
		worldTransform_.rotate.x += kRotSpeed;
	}
}

void Player::Attack() {
	if (Input::GetInstance()->IsTrigger(DIK_J)) {
		// 弾の速度
		const float kBulletSpeed = 1.0f;

		Vector3 velocity = GetReticleWorldPosition() - GetWorldPosition();
		velocity = Normalize(velocity) * kBulletSpeed;

		Model* newModel = new Model();
		newModel->LoadModel("cube");

		bulletModel_.push_back(newModel);

		PlayerBullet* newBullet = new PlayerBullet();

		newBullet->Initialize(newModel, GetWorldPosition(), velocity);

		// 弾を登録する
		bullets_.push_back(newBullet);
	}
}

void Player::Draw() {
	// 弾
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw();
	}

	// プレイヤー
	model_->Draw();

	reticleModel_->Draw();

	sprite2DReticle_->Draw();
}

Vector3 Player::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform_.worldMatrix.m[3][0];
	worldPos.y = worldTransform_.worldMatrix.m[3][1];
	worldPos.z = worldTransform_.worldMatrix.m[3][2];

	return worldPos;
}

Vector3 Player::GetReticleWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldTransform3DReticle_.worldMatrix.m[3][0];
	worldPos.y = worldTransform3DReticle_.worldMatrix.m[3][1];
	worldPos.z = worldTransform3DReticle_.worldMatrix.m[3][2];

	return worldPos;
}

void Player::OnCollision() {
	// 何も起きない
}
