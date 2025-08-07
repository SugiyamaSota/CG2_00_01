#pragma once
#include"../../engine/bonjin/BonjinEngine.h"
#include"../player/Player.h"
#include"../enemy/Enemy.h"
#include"../field/Skydome.h"
#include"../mapchip/MapChipField.h"
#include <list>
#include <memory>

class GameScene {
private:
	//--- カメラ ---
	Camera camera_;
	Vector3 cameraTarget_; // カメラのターゲット座標
	static inline const float kCameraLerpRate = 0.05f; // カメラの追従速度（補間率）

	//--- プレイヤー関連 ---
	std::unique_ptr<Player> player_ = nullptr;
	WorldTransform worldTransform_;
	// モデルをunique_ptrに変更
	std::unique_ptr<Model> model_ = nullptr;

	//--- 敵関連 ---
	std::list<std::unique_ptr<Enemy>> enemies_;
	std::list<std::unique_ptr<Model>> enemyModels_;
	std::list<Enemy*> lockedOnEnemies_;
	static const uint32_t kMaxLockedOnEnemies = 3;

	// --- 天球関連 ---
	std::unique_ptr<Skydome> skydome_ = nullptr;
	// モデルをunique_ptrに変更
	std::unique_ptr<Model> skydomeModel_ = nullptr;

	// --- ブロック関連 ---
	static const uint32_t kNumBlockVirtical = 10;
	static const uint32_t kNumBlockHorizontal = 25;
	Model* blockModel_[kNumBlockVirtical][kNumBlockHorizontal] = { nullptr };
	WorldTransform blockWorldTransform_[kNumBlockVirtical][kNumBlockHorizontal];
	std::unique_ptr<MapChipField> mapChipField_;

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
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
};