#pragma once
#include"engine/bonjin/BonjinEngine.h"

class TitleScene {
private:
	WorldTransform worldTransform_; // ワールドトランスフォーム

	DebugCamera *camera_; // カメラ

	Model* titleModel_ = nullptr; // 3Dモデル

	//終了フラグ
	bool finished_ = false;

public:
	void Initialize();

	~TitleScene();

	void Update();

	void Draw();

	bool IsFinished() const { return finished_; }
};