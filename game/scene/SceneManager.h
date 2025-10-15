#pragma once
#include"TitleScene.h"
#include"GameScene.h"

enum class Scene {
	kTitle,
	kGame,
};

class SceneManager {
private:
	// シーン管理変数
	Scene nowScene = Scene::kTitle;
	Scene nextScene = Scene::kGame;

	// タイトルシーン
	TitleScene* titleScene_ = nullptr;

	// ゲームシーン
	GameScene* gameScene_ = nullptr;

	void ChangeScene();
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneManager();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
};

