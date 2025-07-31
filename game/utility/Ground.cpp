#include "Ground.h"
/// <summary>
/// 初期化
/// </summary>
void Ground::Initialize(Model* model, Camera* camera) {
	worldTransform_ = InitializeWorldTransform();
	camera_ = camera;
	model_ = model;
}

/// <summary>
/// 更新
/// </summary>
void Ground::Update() {
	model_->Update(worldTransform_, camera_, false);

}

/// <summary>
/// 描画
/// </summary>
void Ground::Draw() {
	model_->Draw();
}