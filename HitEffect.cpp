#include "HitEffect.h"
#include<random>

// カメラ
DebugCamera* HitEffect::camera_ = nullptr;

void HitEffect::Initialize(Model* circleModel, std::array<Model*, 2>ellipseModels, Vector3 startPositon) {
	circleWorldTransform_ = {
		{1,1,1},
		{0,0,0},
		startPositon,
	};
	// モデル
	circleModel_ = circleModel;
	ellipseModels_ = ellipseModels;

	// 乱数仮置き
	std::random_device seedGenerator;
	std::mt19937_64 randomEngine;
	randomEngine.seed(seedGenerator());
	float pi = 3.14159265359f;
	std::uniform_real_distribution<float> rotationDistribution(-pi, pi);
	float randNum = rotationDistribution(randomEngine);

	for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
		worldTransform.scale = { 0.0f,0.0f,1.0f };
		worldTransform.rotate = { 0.0f,0.0f,randNum };
		worldTransform.translate = startPositon;
	}

	circleModel_->Update(circleWorldTransform_, { 1,1,1,1 }, camera_);
	for (size_t i = 0; i < ellipseModels_.size(); ++i) {
		if (i < ellipseWorldTransforms_.size()) {
			ellipseModels_[i]->Update(ellipseWorldTransforms_[i], { 1,1,1,1 }, camera_);
		}
	}

	Start();
}

HitEffect* HitEffect::Create(Model* circleModel, std::array<Model*, 2>ellipseModels, Vector3 startPositon) {
	// インスタンス生成
	HitEffect* instance = new HitEffect();
	// newの失敗を検知
	assert(instance);
	//インスタンスを初期化
	instance->Initialize(circleModel, ellipseModels, startPositon);

	// インスタンスを返す
	return instance;
}

HitEffect::~HitEffect() {
	circleModel_ = nullptr; // ポインタを無効化するだけ
	for (size_t i = 0; i < ellipseModels_.size(); ++i) {
		ellipseModels_[i] = nullptr; // ポインタを無効化するだけ
	}
}

void  HitEffect::Update() {
	float t = 0;

	switch (phase_) {
	case Phase::kSpawning:
		t = static_cast<float>(parameter_) / spawnTime_;
		for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
			worldTransform.scale.x = EaseOut(0.0f, 1.0f, t);
			worldTransform.scale.y = EaseOut(0.0f, 10.0f, t);
		}
		// 攻撃動作へ移行
		if (parameter_ >= spawnTime_) {
			phase_ = Phase::kActive;
			parameter_ = 0;
		}
		break;
	case Phase::kActive:
		t = static_cast<float>(parameter_) / activeTime_;
		for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
			worldTransform.scale.x = 1;
			worldTransform.scale.y = 10;
		}
		// 攻撃動作へ移行
		if (parameter_ >= activeTime_) {
			phase_ = Phase::kFinished;
			parameter_ = 0;
		}
		break;
	case Phase::kFinished:
		t = static_cast<float>(parameter_) / finishTime_;
		for (WorldTransform& worldTransform : ellipseWorldTransforms_) {
			worldTransform.scale.x = EaseOut(1.0f, 0.0f, t);
			worldTransform.scale.y = EaseOut(10.0f, 0.0f, t);
		}
		circleWorldTransform_.scale.x = EaseOut(1.0f, 0.0f, t);
		circleWorldTransform_.scale.y = EaseOut(1.0f, 0.0f, t);
		circleWorldTransform_.scale.z = EaseOut(1.0f, 0.0f, t);
		// 攻撃動作へ移行
		if (parameter_ >= finishTime_) {
			phase_ = Phase::kDeath;
			parameter_ = 0;
		}
		break;
	case Phase::kDeath:
		return;
		break;
	}

	parameter_++;

	circleModel_->Update(circleWorldTransform_, { 1,1,1,1 }, camera_);
	for (size_t i = 0; i < ellipseModels_.size(); ++i) {
		if (i <= ellipseWorldTransforms_.size()) {
			ellipseModels_[i]->Update(ellipseWorldTransforms_[i], { 1,1,1,1 }, camera_);
		}
	}
}


void  HitEffect::Draw() {
	if (phase_ == Phase::kDeath) {
		return;
	}

	circleModel_->Draw();
	for (size_t i = 0; i < ellipseModels_.size(); ++i) {
		ellipseModels_[i]->Draw();
	}
}