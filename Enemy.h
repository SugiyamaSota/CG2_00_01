#pragma once
#include"engine/bonjin/BonjinEngine.h"

class Enemy{
private:
	Model* model_ = nullptr;
	WorldTransform worldTransform_;

	const float kMoveSpeed = 0.5f;
	const Vector3 kApproachSpeed = { 0,0,-kMoveSpeed };
	const Vector3 kLeaveSpeed = { kMoveSpeed,0,0 };

	enum class Phase {
		Approach,
		Leave,
	};

	Phase phase_ = Phase::Approach;

	// フェーズごとの関数
	void ApproachPhaseUpdate();
	void LeavePhaseUpdate();

	// メンバ関数ポインタ
	static void(Enemy::* phaseFunctionTable[])();
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

