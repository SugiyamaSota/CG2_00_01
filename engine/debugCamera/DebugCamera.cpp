#include "DebugCamera.h"

void DebugCamera::Initialize(InputKey* inputKey) {
	matRot_ = MakeIdentity4x4();
	translation_ = { 0,0,-50 };

	viewMatrix_ = MakeIdentity4x4();
	projectionMatrix_ = MakeIdentity4x4();

	inputKey_ = inputKey;

	isTargeting_ = false;
	targetPosition_ = { 0,0,0 };
}

void DebugCamera::Update() {
	// 入力による移動や回転

	Move();


	//ビュー行列の更新
	if (!isTargeting_) {
		Rotate();
		Matrix4x4 affineMatrix = MakeAffineMatrix(scale_, matRot_, translation_);
		viewMatrix_ = Inverse(affineMatrix);
	} else if (isTargeting_) {
		viewMatrix_ = MakeLookAtMatrix(translation_, targetPosition_, { 0, 1, 0 });
	}
}

void DebugCamera::Move() {
	// 前移動
	if (inputKey_->IsPress(DIK_C) && !inputKey_->IsPress(DIK_Z)) {
		const float speed = 0.5f;

		//カメラ移動ベクトル
		Vector3 move = { 0,0,speed };
		// 移動ベクトルを回転
		Matrix4x4 rotateMatrix = matRot_;
		move = Conversion(move, rotateMatrix);

		// 座標に加算
		translation_ = Add(translation_, move);
	}

	// 後ろ移動
	if (inputKey_->IsPress(DIK_Z) && !inputKey_->IsPress(DIK_C)) {
		const float speed = -0.5f;

		//カメラ移動ベクトル
		Vector3 move = { 0,0,speed };
		// 移動ベクトルを回転
		Matrix4x4 rotateMatrix = matRot_;
		move = Conversion(move, rotateMatrix);

		// 座標に加算
		translation_ = Add(translation_, move);
	}

	// 上移動
	if (inputKey_->IsPress(DIK_W) && !inputKey_->IsPress(DIK_S)) {
		const float speed = 0.5f;

		//カメラ移動ベクトル
		Vector3 move = { 0,speed,0 };
		// 移動ベクトルを回転
		Matrix4x4 rotateMatrix = matRot_;
		move = Conversion(move, rotateMatrix);

		// 座標に加算
		translation_ = Add(translation_, move);
	}

	// 下移動
	if (inputKey_->IsPress(DIK_S) && !inputKey_->IsPress(DIK_W)) {
		const float speed = -0.5f;

		//カメラ移動ベクトル
		Vector3 move = { 0,speed,0 };
		// 移動ベクトルを回転
		Matrix4x4 rotateMatrix = matRot_;
		move = Conversion(move, rotateMatrix);

		// 座標に加算
		translation_ = Add(translation_, move);
	}

	// 左移動
	if (inputKey_->IsPress(DIK_A) && !inputKey_->IsPress(DIK_D)) {
		const float speed = -0.5f;

		//カメラ移動ベクトル
		Vector3 move = { speed,0,0 };
		// 移動ベクトルを回転
		Matrix4x4 rotateMatrix = matRot_;
		move = Conversion(move, rotateMatrix);

		// 座標に加算
		translation_ = Add(translation_, move);
	}

	// 右移動
	if (inputKey_->IsPress(DIK_D) && !inputKey_->IsPress(DIK_A)) {
		const float speed = 0.5f;

		//カメラ移動ベクトル
		Vector3 move = { speed,0,0 };
		// 移動ベクトルを回転
		Matrix4x4 rotateMatrix = matRot_;
		move = Conversion(move, rotateMatrix);

		// 座標に加算
		translation_ = Add(translation_, move);
	}
}

void DebugCamera::Rotate() {
	// X軸回転
	if (inputKey_->IsPress(DIK_UP) && !inputKey_->IsPress(DIK_DOWN)) {
		Matrix4x4 matRotDelta = MakeIdentity4x4();
		matRotDelta = Multiply(matRotDelta, MakeRotateXMatrix(-rotationSpeed_));
		matRot_ = Multiply(matRotDelta, matRot_);
	}
	if (inputKey_->IsPress(DIK_DOWN) && !inputKey_->IsPress(DIK_UP)) {
		Matrix4x4 matRotDelta = MakeIdentity4x4();
		matRotDelta = Multiply(matRotDelta, MakeRotateXMatrix(rotationSpeed_));
		matRot_ = Multiply(matRotDelta, matRot_);
	}

	// Y軸回転
	if (inputKey_->IsPress(DIK_LEFT) && !inputKey_->IsPress(DIK_RIGHT)) {
		Matrix4x4 matRotDelta = MakeIdentity4x4();
		matRotDelta = Multiply(matRotDelta, MakeRotateYMatrix(-rotationSpeed_));
		matRot_ = Multiply(matRotDelta, matRot_);
	}
	if (inputKey_->IsPress(DIK_RIGHT) && !inputKey_->IsPress(DIK_LEFT)) {
		Matrix4x4 matRotDelta = MakeIdentity4x4();
		matRotDelta = Multiply(matRotDelta, MakeRotateYMatrix(rotationSpeed_));
		matRot_ = Multiply(matRotDelta, matRot_);
	}

	// Z軸回転
	if (inputKey_->IsPress(DIK_E) && !inputKey_->IsPress(DIK_Q)) {
		Matrix4x4 matRotDelta = MakeIdentity4x4();
		matRotDelta = Multiply(matRotDelta, MakeRotateXMatrix(-rotationSpeed_));
		matRot_ = Multiply(matRotDelta, matRot_);
	}
	if (inputKey_->IsPress(DIK_Q) && !inputKey_->IsPress(DIK_E)) {
		Matrix4x4 matRotDelta = MakeIdentity4x4();
		matRotDelta = Multiply(matRotDelta, MakeRotateXMatrix(rotationSpeed_));
		matRot_ = Multiply(matRotDelta, matRot_);
	}
}

Matrix4x4 DebugCamera::MakeLookAtMatrix(const Vector3& eye, const Vector3& target, const Vector3& up) {
	Vector3 zaxis = Normalize(Subtract(target, eye));
	Vector3 xaxis = Normalize(Cross(up, zaxis));
	Vector3 yaxis = Cross(zaxis, xaxis);

	Matrix4x4 result;
	result.m[0][0] = xaxis.x;
	result.m[0][1] = yaxis.x;
	result.m[0][2] = zaxis.x;
	result.m[0][3] = 0.0f;

	result.m[1][0] = xaxis.y;
	result.m[1][1] = yaxis.y;
	result.m[1][2] = zaxis.y;
	result.m[1][3] = 0.0f;

	result.m[2][0] = xaxis.z;
	result.m[2][1] = yaxis.z;
	result.m[2][2] = zaxis.z;
	result.m[2][3] = 0.0f;

	result.m[3][0] = -Dot(xaxis, eye);
	result.m[3][1] = -Dot(yaxis, eye);
	result.m[3][2] = -Dot(zaxis, eye);
	result.m[3][3] = 1.0f;

	return result;
}

void DebugCamera::SetTarget(Vector3 targetPosition) {
	targetPosition_ = targetPosition; isTargeting_ = true;
}

void DebugCamera::ResetRotation() { matRot_ = MakeIdentity4x4(); }