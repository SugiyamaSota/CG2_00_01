#include "EnemyBullet.h"
#include"../player/Player.h"

void EnemyBullet::Initialize(Model* model, const Vector3& position, const Vector3& velocity, Player& player) {
	// モデル
	assert(model);
	model_ = model;

	model_->SetColor({ 1,0,0,1 });

	// ワールド変換
	worldTransform_ = InitializeWorldTransform();
	worldTransform_.translate = position;
	worldTransform_.scale = { 0.5f,0.5f,3.0f };

	velocity_ = velocity;

	worldTransform_.rotate.y= std::atan2f(velocity_.x, velocity_.z);

	float velocityXZ = std::sqrtf(velocity_.x * velocity_.x + velocity_.z * velocity_.z);

	worldTransform_.rotate.x = std::atan2(velocity_.y, velocityXZ);

	worldMatrix_ = MakeIdentity4x4();

	player_ = &player;

	SetCollisionAttibute(kCollisionAttibuteEnemy);
	SetCollisionMask(kCollisionAttibutePlayer);

}

EnemyBullet::~EnemyBullet() {

}

void EnemyBullet::Update(Camera* camera) {
	// ワールド行列
	worldMatrix_ = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);

	// プレイヤー座標との差分健さん
	Vector3 toPlayer = player_->GetWorldPosition() - GetWorldPosition();

	// それぞれの正規化
	toPlayer = Normalize(toPlayer);
	velocity_ = Normalize(velocity_);

	// 速度の球線形補間
	velocity_ = Slerp(velocity_, toPlayer, 5.0f / 60.0f) * 1.0f;

	// 角度計算
	worldTransform_.rotate.y = std::atan2f(velocity_.x, velocity_.z);
	float velocityXZ = std::sqrtf(velocity_.x * velocity_.x + velocity_.z * velocity_.z);
	worldTransform_.rotate.x = std::atan2(velocity_.y, velocityXZ);

	// 一定時間でデス
	if (--deathTimer_ <= 0) {
		isDead_ = true;
	}

	// 速度を適応
	worldTransform_.translate += velocity_;

	// モデルの更新
	model_->Update(worldTransform_, camera, false);
}

void EnemyBullet::Draw() {
	model_->Draw();
}

Vector3 EnemyBullet::GetWorldPosition() {
	Vector3 worldPos;

	worldPos.x = worldMatrix_.m[3][0];
	worldPos.y = worldMatrix_.m[3][1];
	worldPos.z = worldMatrix_.m[3][2];

	return worldPos;
}

void EnemyBullet::OnCollision() {
	// デスフラグをたてる
	isDead_ = true;
}