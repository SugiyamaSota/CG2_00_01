#pragma once
#include"../engine/bonjin/BonjinEngine.h"

#include"player/Player.h"
#include"enemy/Enemy.h"
#include"utility/Collider.h"
#include"utility/Skydome.h"
#include"manager/CollisionManager.h"
#include"utility/RailCameraController.h"
#include <list> // Include list for enemy bullets and enemies
#include<sstream>

class GameScene {
private:
	// カメラ
	Camera* camera_ = nullptr;
	Camera::CameraType cameraType_;

	// カメラコントローラー
	RailCameraController* railCameraController_ = nullptr;

	// プレイヤー
	Model* playerModel_ = nullptr;
	Player* player_ = nullptr;

	// 敵 (複数対応)
	// Model* enemyModel_ = nullptr; // <-- この行は削除
	std::list<Enemy*> enemies_; // Enemyポインタのリスト
	std::list<Model*> enemyModels_; // 各敵のモデルを管理するためのリストを追加

	// 敵弾リスト
	std::list<EnemyBullet*> enemyBullets_;
	std::list<Model*> enemyBulletModels_; // To manage the models created for bullets

	// 天球
	Model* skydomeModel_ = nullptr;
	Skydome* skydome_ = nullptr;

	// 衝突マネージャー
	CollisionManager* collisionManager_ = nullptr;

	// Lineクラスのインスタンス化と初期化
	Line lineRenderer;


	// ベジェ曲線パスの制御点を定義
	std::vector<Vector3> bezierPathPoints = {
		{ 0.0f, 0.0f, 10.0f },
		{ 0.0f, 1.0f, 20.0f },
		{ 0.0f, 2.0f, 30.0f },
		{ 0.0f, 3.0f, 30.0f },
		{ 10.0f, 4.0f, 30.0f },
		{ 20.0f, 5.0f, 20.0f },
		{ 30.0f, 5.0f, 20.0f },
		{ 40.0f, 5.0f, 10.0f },
	};


	std::stringstream enemyPopCommands;

	/// <summary>
	/// 敵を生成し、初期化してリストに追加する
	/// </summary>
	/// <param name="position">敵の初期位置</param>
	/// <param name="modelPath">敵に使用するモデルのパス</param>
	void SpawnEnemy(const Vector3& position, const std::string& modelPath);

	void LoadEnemyPopData();

	void UpdateEnemyPopCommands();

	bool isWaiting_ = false;
	int waitTimer_ = 0;

	// ベジェ曲線関連の変数 (RailCameraControllerに移動するためコメントアウトまたは削除)
	// float bezierT_ = 0.0f; // ベジェ曲線上の現在のパラメータ (0.0 から 1.0)
	// float bezierSpeed_ = 0.005f; // カメラが曲線上を移動する速度
	// float lookAheadDistance_ = 5.0f; // カメラが曲線のどれだけ先を見るか


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

	Camera* GetCamera() { return camera_; }

	// Method to add enemy bullets from Enemy class
	void AddEnemyBullet(EnemyBullet* bullet, Model* model);
};