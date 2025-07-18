#pragma once
#include"engine/bonjin/BonjinEngine.h"
#include<array>


class HitEffect {
private:
	// モデル
	Model* circleModel_;
	std::array<Model*, 2> ellipseModels_;

	// カメラ
	static DebugCamera* camera_;

	// 円のワールドトランスフォーム
	WorldTransform circleWorldTransform_;

	std::array<WorldTransform, 2>ellipseWorldTransforms_;

	enum class Phase {
		kSpawning,
		kActive,
		kFinished,
		kDeath
	};

	Phase phase_ = Phase::kSpawning;

	const uint32_t spawnTime_ = 30;
	const uint32_t activeTime_ = 50;
	const uint32_t finishTime_ = 10;

	uint32_t parameter_ = 0;



public:
	static HitEffect* Create(Model* model, std::array<Model*, 2>ellipseModels, Vector3 startPositon);

	void Initialize(Model* circleModel, std::array<Model*, 2>ellipseModels, Vector3 startPositon);

	void Update();

	void Draw();

	~HitEffect();

	static void SetCamera(DebugCamera* camera) { camera_ = camera; }

	void Start(){ phase_ = Phase::kSpawning; }
};


