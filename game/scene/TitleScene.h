#pragma once
#include"../../engine/bonjin/BonjinEngine.h"

// シーンの状態を表す列挙型
enum class TitlePhase {
	kFadeIn,
	kActive,
	kFadeOut,
};

class TitleScene {
private:
	//--- カメラ ---
	Camera camera_;

	//
	bool isFinished_;

	// フェーズ関連
	TitlePhase phase_ = TitlePhase::kFadeIn;
	float phaseTimer_ = 0.0f;

	// UI
	Sprite* blackScreenSprite_ = nullptr;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	bool GetIsFinished() { return isFinished_; }
};