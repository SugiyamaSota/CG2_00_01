#include "Player.h"
#include<cassert>
#include<algorithm>
#include<cfloat> // FLT_MAX のために必要

#include"../enemy/Enemy.h"
#include"PlayerHormingBullet.h" // PlayerHormingBullet の定義が必要

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
	// ホーミング弾の解放処理も追加
	for (PlayerHormingBullet* bullet : hormingBullets_) {
		delete bullet;
	}
	for (Model* model : hormingBulletModel_) {
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

	// デスフラグが立ったホーミング弾を削除
	hormingBullets_.remove_if([](PlayerHormingBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});

	for (PlayerBullet* bullet : bullets_) {
		bullet->Update(camera);
	}

	// ホーミング弾の更新処理も追加
	for (PlayerHormingBullet* bullet : hormingBullets_) {
		bullet->Update(camera);
	}

	worldTransform_.worldMatrix = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
	if (worldTransform_.parent) {
		worldTransform_.parent->worldMatrix = MakeAffineMatrix(worldTransform_.parent->scale, worldTransform_.parent->rotate, worldTransform_.parent->translate);
		worldTransform_.worldMatrix = Multiply(worldTransform_.worldMatrix, worldTransform_.parent->worldMatrix);
	}

	model_->Update(worldTransform_, camera, false);

	{
		POINT mousePosition;
		GetCursorPos(&mousePosition);
		ScreenToClient(DirectXCommon::GetInstance()->GetHWND(), &mousePosition);

		// positionReticle_ はマウスのクライアント座標として保持
		positionReticle_.x = static_cast<float>(mousePosition.x);
		positionReticle_.y = static_cast<float>(mousePosition.y);

		// 1. マウスのスクリーン座標を正規化デバイス座標 (NDC) に変換
		// ここでビューポートのサイズ (width, height) を正確に使用します
		float viewportWidth = 1280.0f; // あなたのビューポートの幅
		float viewportHeight = 720.0f; // あなたのビューポートの高さ

		Vector3 ndcPosNear = Vector3(
			((positionReticle_.x / viewportWidth) * 2.0f) - 1.0f,
			(((viewportHeight - positionReticle_.y) / viewportHeight) * 2.0f) - 1.0f, // Y軸は多くの場合反転します
			0.0f // Near plane (Z=0 in NDC for DirectX)
		);

		Vector3 ndcPosFar = Vector3(
			((positionReticle_.x / viewportWidth) * 2.0f) - 1.0f,
			(((viewportHeight - positionReticle_.y) / viewportHeight) * 2.0f) - 1.0f,
			1.0f // Far plane (Z=1 in NDC for DirectX)
		);

		// 2. 正規化デバイス座標からワールド座標へ変換
		// ビュープロジェクション行列の逆行列を使用します
		// GetViewProjectionMatrix() は ViewMatrix * ProjectionMatrix のはずです
		Matrix4x4 viewProjectionMatrix = camera->GetViewProjectionMatrix();
		Matrix4x4 inversedViewProjectionMatrix = Inverse(viewProjectionMatrix);

		Vector3 worldPosNear = Transform(ndcPosNear, inversedViewProjectionMatrix);
		Vector3 worldPosFar = Transform(ndcPosFar, inversedViewProjectionMatrix);

		// 3. レイの方向を計算
		Vector3 mouseDirection = Normalize(worldPosFar - worldPosNear);

		// 4. マウスから延ばしたレイ上の3Dレティクルの位置を決定
		const float kDistanceTestObject = 150.0f; // カメラからレティクルまでの距離

		worldTransform3DReticle_.translate = worldPosNear + mouseDirection * kDistanceTestObject;

		// ワールド行列の更新 (スケールと回転は既存の値を使用)
		worldTransform3DReticle_.worldMatrix = MakeAffineMatrix(
			worldTransform3DReticle_.scale,
			worldTransform3DReticle_.rotate,
			worldTransform3DReticle_.translate
		);

		// 親のワールド行列が設定されている場合
		if (worldTransform3DReticle_.parent) {
			// 親のワールド行列を更新し、子に適用
			worldTransform3DReticle_.parent->worldMatrix = MakeAffineMatrix(
				worldTransform3DReticle_.parent->scale,
				worldTransform3DReticle_.parent->rotate,
				worldTransform3DReticle_.parent->translate
			);
			worldTransform3DReticle_.worldMatrix = Multiply(worldTransform3DReticle_.worldMatrix, worldTransform3DReticle_.parent->worldMatrix);
		}
		reticleModel_->Update(worldTransform3DReticle_, camera, false);
	}

	// 2Dレティクル
	{
		sprite2DReticle_->Update(positionReticle_, Color::White);
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
		// 弾のスケール
		Vector3 bulletScale = { 0.25f, 0.25f, 1.5f }; // スケールを定義

		Vector3 bulletSpawnPosition = GetWorldPosition(); // 弾の生成位置

		// ロックオン対象がいるか、かつlockOn_がnullptrではないかを確認
		if (lockOn_ != nullptr && !lockOn_->GetTargets().empty()) {
			// 複数のロックオンターゲットにそれぞれ1発ずつホーミング弾を発射する
			const std::list<Enemy*>& targets = lockOn_->GetTargets();

			for (Enemy* target : targets) {
				// ターゲットが有効かつ死んでいないかを確認
				if (target != nullptr) {
					Vector3 velocity = target->GetWorldPosition() - GetWorldPosition();
					velocity = Normalize(velocity) * kBulletSpeed;

					// ホーミング弾の場合、プレイヤーの少し前から出すなど、生成位置を調整することも可能です
					Vector3 forwardOffset = { 0, 0, 1.0f };
					forwardOffset = TransformNormal(forwardOffset, worldTransform_.worldMatrix);
					// 弾ごとに少しずらして発射することも可能（例: bulletSpawnPosition + Normalize(forwardOffset) * 2.0f + Vector3{0.1f * i, 0, 0};）
					// 今回はシンプルに同じ位置から発射
					Vector3 currentBulletSpawnPosition = bulletSpawnPosition + Normalize(forwardOffset) * 2.0f;

					Model* newModel = new Model();
					newModel->LoadModel("cube"); // ホーミング弾用のモデルロード
					hormingBulletModel_.push_back(newModel);

					PlayerHormingBullet* newBullet = new PlayerHormingBullet();
					newBullet->Initialize(newModel, currentBulletSpawnPosition, velocity, bulletScale, target); // 各ターゲットを渡す
					hormingBullets_.push_back(newBullet);
				}
			}
		} else {
			// ロックオンしていない場合は、プレイヤーの進行方向に弾を出す
			const float kBulletSpeed = 1.0f;
			Vector3 velocity = GetReticleWorldPosition() - GetWorldPosition();
			velocity = Normalize(velocity) * kBulletSpeed;



			velocity = TransformNormal(velocity, worldTransform_.worldMatrix);

			Model* newModel = new Model();
			newModel->LoadModel("cube");

			bulletModel_.push_back(newModel);

			PlayerBullet* newBullet = new PlayerBullet();

			// PlayerBullet::Initialize にもスケール引数が必要になります
			newBullet->Initialize(newModel, bulletSpawnPosition, velocity, bulletScale); // スケールを渡す

			// 弾を登録する
			bullets_.push_back(newBullet);
		}
	}
}


void Player::Draw() {
	// 弾
	for (PlayerBullet* bullet : bullets_) {
		bullet->Draw();
	}
	// ホーミング弾の描画処理も追加
	for (PlayerHormingBullet* bullet : hormingBullets_) {
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

Vector2 Player::GetReticlePosition() {
	Vector2 result;
	result.x = positionReticle_.x;
	result.y = positionReticle_.y;
	return result;
}

void Player::OnCollision() {
	// 何も起きない
}