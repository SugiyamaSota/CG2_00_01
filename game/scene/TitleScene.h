#pragma once
#include"../../engine/bonjin/BonjinEngine.h"
#include"../field/Skydome.h"

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

	Model* titleModel_ = nullptr;
	WorldTransform worldTransform_;

	Model* titleUIModel_ = nullptr;

	// --- 天球関連 ---
	std::unique_ptr<Skydome> skydome_ = nullptr;
	std::unique_ptr<Model> skydomeModel_ = nullptr;

	// UI
	Sprite* blackScreenSprite_ = nullptr;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	~TitleScene();

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