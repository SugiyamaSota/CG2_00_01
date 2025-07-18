#include "Enemy.h"

void Enemy::Initialize(Model* model, const Vector3& startPosition) {
	model_ = model;
	worldTransform_ = InitializeWorldTransform();
	worldTransform_.translate = startPosition;
}

void Enemy::Update(Camera* camera) {
	Vector3 velocity = { 0,0,-kMoveSpeed };

	worldTransform_.translate += velocity;

	model_->Update(worldTransform_, camera, false);
}

void Enemy::Draw() {
	model_->Draw();
}
