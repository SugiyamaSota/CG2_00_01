#pragma once
#include"../../engine/bonjin/BonjinEngine.h"
#include <vector> // std::vector を使用するために追加

class RailCameraController {
private:
	WorldTransform worldTransform_;

	Camera* camera_;

	float cameraSpeed_ = 0.1f;

	// 追加するメンバ変数
	const std::vector<Vector3>* pathPoints_; // ベジェ曲線のパスの制御点へのポインタ
	float currentT_; // パス上の現在のパラメータ (0.0 から 1.0)
	float speed_; // パス上を移動する速度
	float lookAheadDistance_; // 注視点の先行距離
	float verticalOffset_; // 追加: カメラの垂直方向のオフセット (高さ)

public:

	void Initialize(Vector3 worldPosition, float radian);

	// パス情報を設定するための新しいInitialize
	void Initialize(const std::vector<Vector3>* pathPoints, float speed, float lookAheadDistance, float verticalOffset, uint32_t clientWidth, uint32_t clientHeight); // verticalOffset を追加


	void Update(Line* line);

	Matrix4x4 GetViewMatrix() { return camera_->GetViewMatrix(); }
	Vector3 GetTransltate() { return worldTransform_.translate; }
	Vector3 GetRotate() { return worldTransform_.rotate; }
	WorldTransform& GetWorldTransform() { return worldTransform_; }

	// 追加: カメラのWorldTransformを返すゲッター (プレイヤーのSetParentで使用するため)
	const WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// カメラの前方ベクトルを取得するメソッド
	Vector3 GetCameraForwardVector(Line* line) const;
};