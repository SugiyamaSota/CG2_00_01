#include "Enemy.h"

void Enemy::Initialize(Model* model, const Vector3& startPosition) {
	model_ = model;
	worldTransform_ = InitializeWorldTransform();
	worldTransform_.translate = startPosition;
}

void Enemy::Update(Camera* camera) {


	switch (phase_) {
	case Phase::Approach:
	default:
		ApproachPhaseUpdate();
		break;
	case Phase::Leave:
		LeavePhaseUpdate();
		break;
	}

	model_->Update(worldTransform_, camera, false);
}

void Enemy::Draw() {
	model_->Draw();
}

void Enemy::ApproachPhaseUpdate() {
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