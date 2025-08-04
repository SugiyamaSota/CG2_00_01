#pragma once
#include"../../engine/bonjin/BonjinEngine.h"
#include"../player/Player.h"
#include"../field/Skydome.h"
#include"../mapchip/MapChipField.h"

class GameScene{
private:
	// カメラ
	Camera camera_;
	bool isDebugCamera = false;

	//自キャラ
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	Player* player_ = nullptr;
	// 3Dモデル
	Model* model_ = nullptr;

	// ブロック
	static const uint32_t kNumBlockVirtical = 10;
	static const uint32_t kNumBlockHorizontal = 25;
	Model* blockModel_[kNumBlockVirtical][kNumBlockHorizontal] = { nullptr };
	WorldTransform blockWorldTransform_[kNumBlockVirtical][kNumBlockHorizontal];

	Model* goalModel_ = nullptr;
	WorldTransform goalWorldTransform_;
	
	///// マップチップフィールド /////
	MapChipField* mapChipField_;

	//天球
	Skydome* skydome_ = nullptr;
	Model* skydomeModel_ = nullptr;

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

	/// <summary>
	/// ブロック生成
	/// </summary>
	void GenerateBlocks();
};

