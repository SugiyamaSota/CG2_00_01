#include "RailCameraController.h"
#include <cmath>

void RailCameraController::Initialize(Vector3 worldPosition, float radian) {
    worldTransform_ = InitializeWorldTransform();
    worldTransform_.translate = worldPosition;
    worldTransform_.rotate = { 0.0f,radian,0.0f };

    camera_ = new Camera();
    camera_->Initialize(1280, 720); // デフォルトのクライアント幅と高さを設定
}

// 追加するInitializeメソッドの実装
void RailCameraController::Initialize(const std::vector<Vector3>* pathPoints, float speed, float lookAheadDistance, float verticalOffset, uint32_t clientWidth, uint32_t clientHeight) {
    pathPoints_ = pathPoints;
    speed_ = speed;
    lookAheadDistance_ = lookAheadDistance;
    verticalOffset_ = verticalOffset; // verticalOffset を初期化
    currentT_ = 0.0f; // パスの開始点から開始

    worldTransform_ = InitializeWorldTransform(); // WorldTransformの初期化

    camera_ = new Camera();
    camera_->Initialize(clientWidth, clientHeight);
}


void RailCameraController::Update(Line* line) {
    if (pathPoints_ && !pathPoints_->empty()) {
        // 現在のカメラの位置をベジェ曲線上で計算
        Vector3 cameraPosition = line->CalculateBezierPoint(*pathPoints_, currentT_);
        // 垂直オフセットを適用してカメラを上方に持ち上げる
        cameraPosition.y += verticalOffset_;

        float lookAheadT = currentT_ + lookAheadDistance_ / line->GetBezierPathLength(*pathPoints_); // 長さで割ることで正規化されたt値を得る

        // t値をクランプして0.0から1.0の範囲に収める
        if (lookAheadT > 1.0f) {
            lookAheadT = 1.0f;
        }

        Vector3 lookAtPosition = line->CalculateBezierPoint(*pathPoints_, lookAheadT);

        // カメラの位置と注視点に基づいてビュー行列を計算
        camera_->SetViewMatrix(camera_->MakeLookAtMatrix(cameraPosition, lookAtPosition, { 0.0f, 1.0f, 0.0f })); // UpベクトルはY軸固定

        // カメラのWorldTransformを更新
        worldTransform_.translate = cameraPosition;

        // カメラの前方ベクトルを計算
        Vector3 forward = Normalize(lookAtPosition - cameraPosition);

        // 前方ベクトルからヨーとピッチを計算し、worldTransform_.rotate に設定
        // Y軸周りの回転 (ヨー)
        float yaw = atan2f(forward.x, forward.z);
        // X軸周りの回転 (ピッチ)
        float pitch = asinf(-forward.y); // Y軸が上方向の場合、-forward.yでピッチを計算

        worldTransform_.rotate.x = pitch; // ピッチ
        worldTransform_.rotate.y = yaw;   // ヨー
        worldTransform_.rotate.z = 0.0f;  // ロール (通常カメラでは0)


        // currentT_ を更新してカメラを移動させる
        currentT_ += speed_;
        if (currentT_ > 1.0f) {
            currentT_ = 1.0f; // パスの終端に到達したらそれ以上進まない
            // ループさせたい場合は currentT_ = 0.0f; にする
        }
    } else {
        // パスが設定されていない場合、既存のロジックを維持するか、何もしない
        Matrix4x4 worldMatrix = MakeAffineMatrix(worldTransform_.scale, worldTransform_.rotate, worldTransform_.translate);
        camera_->SetViewMatrix(Inverse(worldMatrix));
    }
}

Vector3 RailCameraController::GetCameraForwardVector(Line * line) const {
    if (pathPoints_ && !pathPoints_->empty()) {
        Vector3 cameraPosition = line->CalculateBezierPoint(*pathPoints_, currentT_);
        // ここにも垂直オフセットを適用
        cameraPosition.y += verticalOffset_;

        float lookAheadT = currentT_ + lookAheadDistance_ / line->GetBezierPathLength(*pathPoints_);
        if (lookAheadT > 1.0f) {
            lookAheadT = 1.0f;
        }
        Vector3 lookAtPosition = line->CalculateBezierPoint(*pathPoints_, lookAheadT);

        // カメラの前方ベクトルは、注視点からカメラ位置へのベクトル
        Vector3 forward = Normalize(lookAtPosition - cameraPosition);
        return forward;
    }
    // パスが設定されていない場合は、デフォルトの前方ベクトルを返すか、エラー処理を行う
    return { 0.0f, 0.0f, -1.0f };
}