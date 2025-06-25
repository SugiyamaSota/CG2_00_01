#include "Camera.h"

#include"../input/Input.h"
#include <cmath>      // std::sin, std::cos, std::atan2, std::fmod など
#include <algorithm>  // std::clamp のため

// 定数としてPIを定義 (必要に応じてヘッダーファイルに移動)
const float PI = 3.1415926535f;

// 度数法からラジアンへの変換ヘルパー関数
// MyMath.h にこのような関数があればそれを使用してください
float DegToRad(float deg) {
	return deg * (PI / 180.0f);
}

void Camera::Initialize(uint32_t clientWidth, uint32_t clientHeight) {
	// 従来のmatRot_とtranslation_は球面座標から計算されるため、初期値は球面座標で設定
	// matRot_ = MakeIdentity4x4(); // CalculateCameraPositionAndRotationで設定される
	// translation_ = { 0,0,-50 }; // CalculateCameraPositionAndRotationで設定される

	// ▼▼▼ 球面座標系の初期値設定 ▼▼▼
	radius_ = 50.0f;           // 初期半径 (元々の-50Zの距離に対応)
	theta_ = DegToRad(0.0f);    // 初期方位角 (Y軸周り、0度はZ+方向)
	phi_ = DegToRad(90.0f);     // 初期極角 (X軸周り、90度はXZ平面上)
	targetPosition_ = { 0,0,0 }; // 初期注視点
	isTargeting_ = true;       // 常に注視点を見ている状態にする
	// ▲▲▲ 球面座標系の初期値設定 ▲▲▲

	viewMatrix_ = MakeIdentity4x4();
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(clientWidth) / float(clientHeight), 0.1f, 1000.0f); // farクリップ距離を1000.0fに延長

	// isTargeting_ = false; // trueに変更
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

	CalculateCameraPositionAndRotation();

	viewMatrix_ = MakeLookAtMatrix(translation_, targetPosition_, { 0, 1, 0 });

	viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
}

void Camera::Move() {
	Input* input = Input::GetInstance();

	// マウスホイールによるズーム (半径の変更)
	if (input->GetMouseWheel() != 0) {
		long mouseDeltaZ = input->GetMouseWheel();
		const float zoomSpeed = 0.01f; // ズーム速度を調整

		radius_ -= static_cast<float>(mouseDeltaZ) * zoomSpeed; // ホイール奥でズームイン、手前でズームアウト
		// 半径の最小値と最大値を制限
		radius_ = std::clamp(radius_, 1.0f, 500.0f); // 最小1.0f、最大500.0fに制限
	}

	// マウスホイールボタン (ボタン2) と左Shiftキーが同時に押されている場合 (パン機能)
	// マウスを左右に動かすと左右に、上下に動かすと上下にカメラが平行移動する
	if (input->IsMousePress(2) && input->IsPress(DIK_LSHIFT)) {
		long mouseDeltaX = input->GetMouseDeltaX();
		long mouseDeltaY = input->GetMouseDeltaY();

		const float panSpeed = 0.01f; // パン速度を調整

		// カメラのローカル座標系での移動ベクトル
		Vector3 moveLocal = {
			static_cast<float>(-mouseDeltaX) * panSpeed, // X軸方向 (左右)
			static_cast<float>(mouseDeltaY) * panSpeed,  // Y軸方向 (上下)
			0.0f                                       // Z軸方向はここでは動かさない
		};

		// ローカル移動ベクトルをワールド座標系に変換
		// カメラの現在の向きに応じて移動方向が変わるようにする
		Matrix4x4 cameraWorldMatrix = Inverse(viewMatrix_); // ビュー行列の逆行列がカメラのワールド行列
		Vector3 worldRight = { cameraWorldMatrix.m[0][0], cameraWorldMatrix.m[0][1], cameraWorldMatrix.m[0][2] }; // カメラの右ベクトル
		Vector3 worldUp = { cameraWorldMatrix.m[1][0], cameraWorldMatrix.m[1][1], cameraWorldMatrix.m[1][2] };     // カメラの上ベクトル

		// worldRightとworldUpを使ってtargetPosition_を更新
		targetPosition_ = Add(targetPosition_, Multiply(moveLocal.x, worldRight));
		targetPosition_ = Add(targetPosition_, Multiply(moveLocal.y, worldUp));
	}
}

void Camera::Rotate() {
	Input* input = Input::GetInstance();

	// マウスホイールボタン (ボタン2) が押されており、かつ左Shiftキーが押されていない場合、回転
	if (input->IsMousePress(2) && !input->IsPress(DIK_LSHIFT)) {
		long mouseDeltaX = input->GetMouseDeltaX();
		long mouseDeltaY = input->GetMouseDeltaY();

		// マウスの左右移動で方位角 (theta_) を更新 (Y軸回転)
		theta_ += static_cast<float>(mouseDeltaX) * rotationSpeed_; // 符号は調整が必要かもしれません

		// マウスの上下移動で極角 (phi_) を更新 (X軸回転)
		phi_ += static_cast<float>(-mouseDeltaY) * rotationSpeed_; // 符号は調整が必要かもしれません

		const float MIN_PHI = DegToRad(1.0f);     // 1度
		const float MAX_PHI = DegToRad(179.0f);   // 179度
		phi_ = std::clamp(phi_, MIN_PHI, MAX_PHI);

	}
}

// ▼▼▼ 新しく追加する関数 ▼▼▼
void Camera::CalculateCameraPositionAndRotation() {
	// 球面座標からデカルト座標への変換
	// Y+が上方向の座標系を想定 (DirectXに近い)

	// XZ平面での投影された半径
	float projectedRadius = radius_ * std::sin(phi_);

	// カメラのワールド座標を計算
	translation_.x = targetPosition_.x + projectedRadius * std::sin(theta_);
	translation_.z = targetPosition_.z + projectedRadius * std::cos(theta_);
	translation_.y = targetPosition_.y + radius_ * std::cos(phi_);

	// matRot_はMakeLookAtMatrixでビュー行列が計算されるため、
	// 明示的に更新する必要は通常ありません。
	// もしmatRot_がカメラのワールド行列の回転部分として使われるなら、
	// ここで計算することもできますが、LookAtカメラでは通常直接viewMatrix_を設定します。
	// 例えば、
	// matRot_ = Multiply(MakeRotateYMatrix(theta_), MakeRotateXMatrix(phi_));
	// のように計算することも可能ですが、この場合はXとYの軸の定義に注意が必要です。
	// 今回はMakeLookAtMatrixを使用するため、matRot_は直接View行列には影響しません。
	// affineMatrixで利用しているため、念のため更新はしておきます。
	matRot_ = Multiply(MakeRotateYMatrix(theta_), MakeRotateXMatrix(phi_));
}

// ResetPositionとResetRotationも球面座標系に合わせて変更
void Camera::ResetPosition() {
	radius_ = 50.0f;
	targetPosition_ = { 0,0,0 };
}

void Camera::ResetRotation() {
	theta_ = DegToRad(0.0f);
	phi_ = DegToRad(90.0f);
}
// ▲▲▲ 新しく追加する関数 ▲▲▲

// MakeLookAtMatrix は変更なしでOK
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