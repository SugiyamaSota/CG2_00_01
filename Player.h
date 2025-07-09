#pragma once
#include"engine/bonjin/BonjinEngine.h"
#include<list>

#include"PlayerBullet.h"

class Player{
private:
	// モデル
	Model* model_ = nullptr;

	// ワールド変換
	WorldTransform worldTransform_{};

	// 弾
	std::list<PlayerBullet*> bullets_;
	std::list<Model*> bulletModel_;

	/// <summary>
	/// 移動処理
	/// </summary>
	void Move();

	/// <summary>
	/// 旋回処理
	/// </summary>
	void Rotate();

	/// <summary>
	/// 攻撃処理
	/// </summary>
	void Attack();

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Camera* camera);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
};

