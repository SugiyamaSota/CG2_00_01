#pragma once
#include"engine/bonjin/BonjinEngine.h"

#include"Player.h"
#include"Enemy.h"

class GameScene {
private:
	// カメラ
	Camera* camera_ = nullptr;
	Camera::CameraType cameraType_;

	// プレイヤー
	Model* playerModel_ = nullptr;
	Player* player_ = nullptr;

	// 敵
	Model* enemyModel_ = nullptr;
	Enemy* enemy_ = nullptr;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(uint32_t clientWidth, uint32_t clientHeight);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
};