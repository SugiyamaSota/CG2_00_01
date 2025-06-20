#pragma once
#include"engine/bonjin/BonjinEngine.h"
#include"Player.h"
#include"Skydome.h"

class GameScene{
private:
	//自キャラ
	// ワールドトランスフォーム
	WorldTransform worldTransform_;
	// カメラ
	DebugCamera camera_;
	Player* player_ = nullptr;
	// 3Dモデル
	Model* model_ = nullptr;

	// ブロック
	static const uint32_t kNumBlockVirtical = 10;
	static const uint32_t kNumBlockHorizontal = 20;
	Model* blockModel_[kNumBlockVirtical][kNumBlockHorizontal] = { nullptr };
	WorldTransform blockWorldTransform_[kNumBlockVirtical][kNumBlockHorizontal];
	
	//天球
	Skydome* skydome_ = nullptr;
	Model* skydomeModel_ = nullptr;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(InputKey* key);

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

