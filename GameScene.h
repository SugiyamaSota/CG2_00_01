#pragma once
#include"engine/bonjin/BonjinEngine.h"
#include"Player.h"
#include"Enemy.h"
#include"Skydome.h"
#include"MapChipField.h"
#include"Deathparticles.h"

class GameScene{
private:
	// カメラ
	DebugCamera camera_;
	bool isDebugCamera = false;

	//自キャラ
	WorldTransform worldTransform_;
	Player* player_ = nullptr;
	Model* model_ = nullptr;

	///// プレイヤーのパーティクル /////
	DeathParticles* deathParticles_ = nullptr; // 死亡時
	bool deathParticlesExistFlag = true;

	///// 敵 /////Add commentMore actions
	Enemy* enemy_ = nullptr;
	Model* enemyModel_ = nullptr;

	// ブロック
	static const uint32_t kNumBlockVirtical = 10;
	static const uint32_t kNumBlockHorizontal = 25;
	Model* blockModel_[kNumBlockVirtical][kNumBlockHorizontal] = { nullptr };
	WorldTransform blockWorldTransform_[kNumBlockVirtical][kNumBlockHorizontal];
	
	///// マップチップフィールド /////
	MapChipField* mapChipField_;

	//天球
	Skydome* skydome_ = nullptr;
	Model* skydomeModel_ = nullptr;

	enum class Phase {
		kPlay,
		kDeath,
	};

	Phase phase_;

	void ChangePhase();

	bool finished_ = false;

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

	//すべての当たり判定Add commentMore actions
	void CheckAllCollisions();

	bool IsFinished() const { return finished_; }
};

