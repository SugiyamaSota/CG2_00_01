#pragma once
#include"../../engine/bonjin/BonjinEngine.h"
#include"../player/Player.h"
#include"../enemy/Enemy.h"
#include"../field/Skydome.h"
#include"../mapchip/MapChipField.h"

class GameScene {
private:
	//--- カメラ ---
	Camera camera_;

	//--- プレイヤー関連 ---
	Player* player_ = nullptr;
	WorldTransform worldTransform_;
	Model* model_ = nullptr;

	//--- 敵関連 ---
	Enemy* enemy_ = nullptr;
	Model* enemyModel_ = nullptr;

	// --- 天球関連 ---
	Skydome* skydome_ = nullptr;
	Model* skydomeModel_ = nullptr;

	// --- ブロック関連 ---
	static const uint32_t kNumBlockVirtical = 10;
	static const uint32_t kNumBlockHorizontal = 25;
	Model* blockModel_[kNumBlockVirtical][kNumBlockHorizontal] = { nullptr };
	WorldTransform blockWorldTransform_[kNumBlockVirtical][kNumBlockHorizontal];
	MapChipField* mapChipField_;

	/// <summary>
	/// ブロック生成
	/// </summary>
	void GenerateBlocks();

	/// <summary>
	/// 当たり判定
	/// </summary>
	void CheckAllCollisions();
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

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

