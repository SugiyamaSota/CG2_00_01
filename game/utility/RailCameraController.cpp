#include "RailCameraController.h"

void RailCameraController::Initialize(Vector3 worldPosition, float radian) {
	worldTransform_ = InitializeWorldTransform();
	worldTransform_.translate = worldPosition;
	worldTransform_.rotate = { 0.0f,radian,0.0f };

	camera_ = new Camera();
	camera_->Initialize(1280, 720);
}

void RailCameraController::Update() {
	worldTransform_.translate.z -= cameraSpeed_;
	
	Matrix4x4 worldMatrix = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);

	camera_->SetViewMatrix(Inverse(worldMatrix));
}
