#include "TitleScene.h"

using namespace BonjinEngine;

void TitleScene::Initialize(Camera* camera) {
    currentSceneType_ = SceneType::kTitle;
    nextSceneType_ = SceneType::kTitle;

    this->camera_ = camera;

    WorldTransform wt = InitializeWorldTransform();
    model_ = new Model;
    model_->LoadModel("teapot");
    model_->Update(wt, camera);
}

void TitleScene::Unload() {
    if (model_ != nullptr) {
        delete model_;
        model_ = nullptr;
    }
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

void TitleScene::DrawImGui() {

}

SceneType TitleScene::GetNextScene() const {
    return nextSceneType_;
}