#pragma once
#include "engine/bonjin/BonjinEngine.h"
#include <cassert>

/// <summary>
/// 自キャラ
/// </summary>
class Player {
private:
	// ワールドトランスフォーム
	WorldTransform worldTransform_;

	//カメラ
	DebugCamera* camera_ = nullptr;

	// 3Dモデル
	Model* model_ = nullptr;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model, DebugCamera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
};