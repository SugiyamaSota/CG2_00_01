#pragma once
#include"engine/bonjin/BonjinEngine.h"

class Enemy{
private:
	Model* model_ = nullptr;
	WorldTransform worldTransform_;

	const float kMoveSpeed = 1.0f;
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Model* model,const Vector3& startPosition);


	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(Camera* camera);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
};

