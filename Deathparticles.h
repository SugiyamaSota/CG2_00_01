#pragma once
#include"engine/bonjin/BonjinEngine.h"
#include<array>

class DeathParticles {
private:
	//パーティクルの個数
	static inline const uint32_t kNumParticles = 8;

	Model model_[kNumParticles];

	// カメラ
	DebugCamera* camera_;


	WorldTransform worldTransforms_[kNumParticles];

	//存在時間(秒)
	static inline const float kDuration = 2;

	//移動の速さ
	static inline const float kSpeed = 0.1f;

	Vector3 velocities_[kNumParticles];

	//分割した1個分の角度
	static inline const float pi = float(3.14159265359);
	static inline const float kAngleUnit = (2 * pi) / kNumParticles;

	//終了フラグ
	bool isFinished_ = false;
	//時間経過カウント
	float counter_ = 0.0f;

	//色の数値
	Vector4 color_;

public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(const std::string& modelFileName, DebugCamera* camera, const Vector3& position);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
};