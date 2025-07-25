#pragma once
#include"../../engine/bonjin/BonjinEngine.h"

class RailCameraController{
private:
	WorldTransform worldTransform_;

	Camera* camera_;

	float cameraSpeed_ = 0.1f;
public:

	void Initialize(Vector3 worldPosition, float radian);

	void Update();

	Matrix4x4 GetViewMatrix() { return camera_->GetViewMatrix(); }
	Vector3 GetTransltate() { return worldTransform_.translate; }
	Vector3 GetRotate() { return worldTransform_.rotate; }
	WorldTransform& GetWorldTransform() { return worldTransform_; }
};

