#include "Skydome.h"
/// <summary>
/// 初期化
/// </summary>
void Skydome::Initialize(Model* model, Camera* camera) {
	worldTransform_ = InitializeWorldTransform();
	camera_ = camera;
	model_ = model;
}

/// <summary>
/// 更新
/// </summary>
void Skydome::Update() {
	model_->Update(worldTransform_, camera_, false);
}

/// <summary>
/// 描画
/// </summary>
void Skydome::Draw() {
	model_->Draw();
}