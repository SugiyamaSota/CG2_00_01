#include "TitleScene.h"

using namespace BonjinEngine;

void TitleScene::Initialize(Camera* camera) {
    // 今のシーンと遷移後シーン(初期値は同じ)
    currentSceneType_ = SceneType::kTitle;
    nextSceneType_ = SceneType::kTitle;

    this->camera_ = camera;

    WorldTransform wt = InitializeWorldTransform();
    model_ = new Model;
    model_->LoadModel("teapot");
    model_->Update(wt, camera);
}




void TitleScene::Update(float deltaTime) {
    WorldTransform wt = InitializeWorldTransform();
    model_->Update(wt, camera_);

    if (Input::GetInstance()->IsTrigger(DIK_SPACE)) {
        nextSceneType_ = SceneType::kGame;
    }
}

void TitleScene::Draw() {
    model_->Draw();
}

SceneType TitleScene::GetNextScene() const {
    return nextSceneType_;
}