#include "TitleScene.h"


void TitleScene::Initialize() {
	titleModel_ = new Model();
	titleModel_->LoadModel("title");

	
	worldTransform_ = {
		{1,1,1},
		{0,0,0},
		{1,1,1},
	};

	camera_ = new DebugCamera();
	camera_->Initialize();
	
}

TitleScene::~TitleScene() {
	delete titleModel_;
	delete camera_;
}

void TitleScene::Update() {
	if (InputKey::GetInstance()->IsTrigger(DIK_SPACE)) {
		finished_ = true;
	}
	camera_->Update(true);
	titleModel_->Update(worldTransform_, { 1,1,1,1 }, camera_);
}

void TitleScene::Draw() {

	titleModel_->Draw();

}