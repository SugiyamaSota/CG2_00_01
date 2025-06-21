#include "DeathParticles.h"
#define NOMINMAX
#include <algorithm>

void DeathParticles::Initialize(const std::string& modelFileName, DebugCamera* camera, const Vector3& position) {
	camera_ = camera;
	// assert(model); // Model* 引数がなくなったので不要

	for (uint32_t i = 0; i < kNumParticles; i++) {
		// ★ 各パーティクルに新しいModelインスタンスをロードする
		model_[i].LoadModel(modelFileName); // 各Modelインスタンスが個別のリソースを持つく

		worldTransforms_[i] = {
			{0.5f,0.5f,0.5f},
			{0,0,0},
			position,
		};
		// 各パーティクルの初期速度を計算し、velocities_に保存
		Vector3 baseVelocity = { kSpeed, 0, 0 };
		float angle = kAngleUnit * i;
		Matrix4x4 matrixRotation = MakeRotateZMatrix(angle);
		velocities_[i] = Conversion(baseVelocity, matrixRotation);
	}
	color_ = { 1, 1, 1, 1 };
}

void DeathParticles::Update() {
	// 終了なら何もしない
	if (isFinished_) {
		return;
	}

	for (uint32_t i = 0; i < kNumParticles; ++i) {
		// 保存しておいた速度ベクトルで移動処理を行う
		worldTransforms_[i].translate = Add(worldTransforms_[i].translate, velocities_[i]);
	}

	// カウンターを１フレーム分の秒数進める
	counter_ += 1.0f / 60.0f;

	// 存続時間の上限に達したら
	if (counter_ >= kDuration) {
		counter_ = kDuration;
		isFinished_ = true;
	}

	color_.w = std::clamp(1.0f - (1.0f * counter_), 0.0f, 1.0f);

	// 行列変換
	for (uint32_t i = 0; i < kNumParticles; ++i) {
		model_[i].Update(worldTransforms_[i],color_, camera_);
	}
}

void DeathParticles::Draw() {
	// 終了なら何もしない
	if (isFinished_) {
		return;
	}
	for (uint32_t i = 0; i < kNumParticles; ++i) {
		model_[i].Draw();
	}
}