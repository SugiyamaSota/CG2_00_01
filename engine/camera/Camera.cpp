#include "Camera.h"

#include"../input/Input.h"

void Camera::Initialize(uint32_t clientWidth, uint32_t clientHeight) {
	matRot_ = MakeIdentity4x4();
	translation_ = { 0,0,-50 };

	viewMatrix_ = MakeIdentity4x4();
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(clientWidth) / float(clientHeight), 0.1f, 100.0f);

	isTargeting_ = false;
	targetPosition_ = { 0,0,0 };
}

void Camera::Update(CameraType type) {
	if (Input::GetInstance()->IsTrigger(DIK_R)) {
		ResetPosition();
		ResetRotation();
	}
	switch (type) {
	case CameraType::kNormal:
		break;
	case CameraType::kDebug:
		Move();
		Rotate();
		break;
	}
	// ビュー行列の更新
	if (!isTargeting_) {
		Matrix4x4 affineMatrix = MakeAffineMatrix(scale_, matRot_, translation_);
		viewMatrix_ = Inverse(affineMatrix);
	} else if (isTargeting_) {
		viewMatrix_ = MakeLookAtMatrix(translation_, targetPosition_, { 0, 1, 0 });
	}
	// ビュープロジェクション行列の作成
	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

void Camera::Move() {
	// Inputインスタンスを一度だけ取得してキャッシュ
	Input* input = Input::GetInstance();

	// マウスホイールによる前後移動
	if (input->GetMouseWheel() != 0) {
		long mouseDeltaZ = input->GetMouseWheel();
		const float speed = 0.01f;

		// カメラ移動ベクトル（Z軸方向のみ）
		// DirectInputのGetMouseWheel()は、手前にスクロールすると負、奥にスクロールすると正のことが多いですが、
		// 動作を見て前後を調整してください。ここではmouseDeltaZが正なら前進としています。
		Vector3 move = { 0, 0, speed * static_cast<float>(mouseDeltaZ) };

		// 移動ベクトルをカメラの現在の回転に合わせて変換
		// これにより、カメラが向いている方向に前後に移動します。
		Matrix4x4 rotateMatrix = matRot_;
		move = Conversion(move, rotateMatrix);

		// 座標に加算
		translation_ = Add(translation_, move);
	}

	// マウスホイールボタン (ボタン2) と左Shiftキーが同時に押されている場合
	if (input->IsMousePress(2) && input->IsPress(DIK_LSHIFT)) {
		long mouseDeltaX = input->GetMouseDeltaX();
		long mouseDeltaY = input->GetMouseDeltaY(); // マウスの上下移動量

		const float speed = 0.05f; // 移動速度を調整 (マウス移動量に対する感度)

		Vector3 move = {
			static_cast<float>(-mouseDeltaX) * speed, // X軸方向の移動 (左右)
			static_cast<float>(-mouseDeltaY) * speed, // Y軸方向の移動 (上下)
			0.0f                                     // Z軸方向はここでは動かさない
		};

		// 移動ベクトルをカメラの現在の回転に合わせて変換
		// これが重要です！カメラがどちらを向いていても、そのカメラの
		// ローカルな左右・上下方向に移動させるために必要です。
		Matrix4x4 rotateMatrix = matRot_;
		move = Conversion(move, rotateMatrix);

		// 座標に加算
		translation_ = Add(translation_, move);
	}
}

void Camera::Rotate() {
	// Inputインスタンスを一度だけ取得してキャッシュ
	Input* input = Input::GetInstance();

	// マウスホイールボタン (ボタン2) が押されており、かつ左Shiftキーが押されていない場合
	if (input->IsMousePress(2) && !input->IsPress(DIK_LSHIFT)) { // !input->IsPress(DIK_LSHIFT) を使用
		long mouseDeltaX = input->GetMouseDeltaX();
		long mouseDeltaY = input->GetMouseDeltaY();

		// マウスのX軸移動によるY軸回転 (左右の首振り)
		if (mouseDeltaX != 0) {
			Matrix4x4 matRotDeltaY = MakeRotateYMatrix(static_cast<float>(-mouseDeltaX) * rotationSpeed_);
			matRot_ = Multiply(matRotDeltaY, matRot_);
		}

		// マウスのY軸移動によるX軸回転 (上下の首振り)
		if (mouseDeltaY != 0) {
			Matrix4x4 matRotDeltaX = MakeRotateXMatrix(static_cast<float>(-mouseDeltaY) * rotationSpeed_);
			matRot_ = Multiply(matRotDeltaX, matRot_);
		}
	}
}

Matrix4x4 Camera::MakeLookAtMatrix(const Vector3& eye, const Vector3& target, const Vector3& up) {
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

void Camera::SetTarget(Vector3 targetPosition) {
	targetPosition_ = targetPosition; isTargeting_ = true;
}