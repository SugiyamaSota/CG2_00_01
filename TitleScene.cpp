#include "TitleScene.h"


void TitleScene::Initialize() {
	worldTransform_ = {
		{1,1,1},
		{0,0,0},
		{1,1,1},
	};
	camera_ = new DebugCamera();
	camera_->Initialize();
	
	titleModel_ = new Model();
	titleModel_->LoadModel("title");
	titleModel_->Update(worldTransform_, { 1,1,1,1 }, camera_);


	fade_ = new Fade();
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, 1.0f);
}

TitleScene::~TitleScene() {
	delete titleModel_;
	delete camera_;
	delete fade_;
}

void TitleScene::Update() {
	camera_->Update(true);
	titleModel_->Update(worldTransform_, { 1,1,1,1 }, camera_);
	switch (phase_) {
	case Phase::kFadeIn:
		fade_->Update();
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;
	case Phase::kMain:
		if (InputKey::GetInstance()->IsTrigger(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, 1);
		}
		break;
	case Phase::kFadeOut:
		fade_->Update();
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
}

void TitleScene::Draw() {

	titleModel_->Draw();
	fade_->Draw();
}