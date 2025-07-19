#include "Enemy.h"

void(Enemy::* Enemy::phaseFunctionTable[])() = {
	&Enemy::ApproachPhaseUpdate,
	&Enemy::LeavePhaseUpdate
};

void Enemy::Initialize(Model* model, const Vector3& startPosition) {
	model_ = model;
	worldTransform_ = InitializeWorldTransform();
	worldTransform_.translate = startPosition;

	ApproachPhaseInitialize();
}

Enemy::~Enemy() {
	for (EnemyBullet* bullet : bullets_) {
		delete bullet;
	}
	for (Model* model : bulletModel_) {
		delete model;
	}
}

void Enemy::Update(Camera* camera) {

	(this->*phaseFunctionTable[static_cast<size_t>(phase_)])();

	model_->Update(worldTransform_, camera, false);

	for (EnemyBullet* bullet : bullets_) {
		bullet->Update(camera);
	}
}

void Enemy::Draw() {
	model_->Draw();

	for (EnemyBullet* bullet : bullets_) {
		bullet->Draw();
	}
}

void Enemy::ApproachPhaseInitialize() {
	fireTimer_ = kFireInterval;
}

void Enemy::ApproachPhaseUpdate() {
	// カウントダウン
	fireTimer_--;

	// 指定時間に達した
	if (fireTimer_ <= 0) {
		Fire();;

		fireTimer_ = kFireInterval;
	}

	// 速度を加算
	worldTransform_.translate += kApproachSpeed;
	// 一定座標に来たら離脱
	if (worldTransform_.translate.z < 0.0f) {
		phase_ = Phase::Leave;
	}
}

void Enemy::LeavePhaseUpdate() {
	// 速度を加算
	worldTransform_.translate += kLeaveSpeed;
}

void Enemy::Fire() {
	// 弾の速度
	const float kBulletSpeed = -1.0f;
	Vector3 velocity = { 0,0,kBulletSpeed };

	Matrix4x4 worldMatrix = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);

	velocity = TransformNormal(velocity, worldMatrix);

	Model* newModel = new Model();
	newModel->LoadModel("cube");

	bulletModel_.push_back(newModel);

	EnemyBullet* newBullet = new EnemyBullet();
	newBullet->Initialize(newModel, worldTransform_.translate, velocity);

	// 弾を登録する
	bullets_.push_back(newBullet);
}