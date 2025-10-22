#include "GameScene.h"

using namespace BonjinEngine;

void GameScene::Initialize(Camera * camera) {
    // 今のシーンと遷移後シーン(初期値は同じ)
    currentSceneType_ = SceneType::kGame;
    nextSceneType_ = SceneType::kGame;

    this->camera_ = camera;

    WorldTransform wt = InitializeWorldTransform();
    model_ = new Model;
    model_->LoadModel("axis");
    model_->Update(wt, camera);
}




void GameScene::Update(float deltaTime) {
    WorldTransform wt = InitializeWorldTransform();
    model_->Update(wt, camera_);

    if (Input::GetInstance()->IsTrigger(DIK_SPACE)) {
        nextSceneType_ = SceneType::kTitle;
    }
}

void GameScene::Draw() {
    model_->Draw();
}

SceneType GameScene::GetNextScene() const {
    return nextSceneType_;
}