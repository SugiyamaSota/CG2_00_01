#include "Skydome.h"
/// <summary>
/// 初期化
/// </summary>
void Skydome::Initialize(Model* model, Camera* camera) {
	worldTransform_.Initialize();
	camera_ = camera;
	model_ = model;
}

/// <summary>
/// 更新
/// </summary>
void Skydome::Update() {

}

/// <summary>
/// 描画
/// </summary>
void Skydome::Draw() {
	model_->Draw(worldTransform_, *camera_);
}