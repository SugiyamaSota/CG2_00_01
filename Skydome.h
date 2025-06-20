#pragma once
#include "engine/bonjin/BonjinEngine.h"

class Skydome {
private:
	//ワールド変換
	WorldTransform worldTransform_;
	//モデル
	Model* model_ = nullptr;
	// カメラ
	DebugCamera* camera_ = nullptr;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model,DebugCamera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
};