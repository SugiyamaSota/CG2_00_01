#include "GameScene.h"

using namespace BonjinEngine;

void GameScene::Initialize() {
    // 今のシーンと遷移後シーン(初期値は同じ)
    currentSceneType_ = SceneType::kGame;
    nextSceneType_ = SceneType::kGame;

    model_ = new Model;
    model_->LoadModel("axis");
}




void GameScene::Update(float deltaTime, Camera camera) {
    WorldTransform wt = InitializeWorldTransform();
    model_->Update(wt, &camera);
}

void GameScene::Draw() {
    model_->Draw();
}

SceneType GameScene::GetNextScene() const {
    return nextSceneType_;
}