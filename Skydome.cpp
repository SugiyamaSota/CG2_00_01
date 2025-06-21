#include "Skydome.h"


/// <summary>
/// 初期化
/// </summary>
void Skydome::Initialize(Model* model,DebugCamera* camera) {
	worldTransform_ = {
		{1,1,1},
		{0,0,0},
		{0,0,0},
	};
	camera_ = camera;
	model_ = model;
}

/// <summary>
/// 更新
/// </summary>
void Skydome::Update() {
	model_->Update(worldTransform_, { 1,1,1,1 }, camera_);
}

/// <summary>
/// 描画
/// </summary>
void Skydome::Draw() {
	model_->Draw();
}