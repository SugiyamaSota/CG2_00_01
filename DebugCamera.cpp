#include "DebugCamera.h"


#include "MyMath/Vector.h"
#include "MyMath/Convert.h"
#include "MyMath/Matrix.h"


void DebugCamera::Initialize(InputKey *inputKey) {
	matRot_ = MakeIdentity4x4();
	translation_ = { 0,0,-50 };

	viewMatrix_ = MakeIdentity4x4();
	projactionMatirx_ = MakeIdentity4x4();

	inputKey_ = inputKey;
}

void DebugCamera::Update() {

	// 入力による移動や回転
	Rotate();
	Move();

	//ビュー行列の更新
	Matrix4x4 affineMatrix = MakeAffineMatrix(scale_, matRot_, translation_);
	viewMatrix_ = Inverse(affineMatrix);
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