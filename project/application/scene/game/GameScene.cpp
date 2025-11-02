#include "GameScene.h"

using namespace BonjinEngine;

void GameScene::Initialize(Camera * camera) {
    // 今のシーンと遷移後シーン(初期値は同じ)
    currentSceneType_ = SceneType::kGame;
    nextSceneType_ = SceneType::kGame;

    this->camera_ = camera;

  /*  particle_ = new Particle;
    particle_->LoadModel("plane");
    particle_->Emit({ 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f }, 5.0f, 1.0f, 3.0f);*/
}




void GameScene::Update(float deltaTime) {
   // particle_->Update(camera_);

    int num = RandomEngine::GetInstance()->Rand(0, 5);

    ImGui::Begin("Debug::Random");
    ImGui::Text("randomnum : 0~5 -> %d", num);
    ImGui::End();

    if (Input::GetInstance()->IsTrigger(DIK_SPACE)) {
        nextSceneType_ = SceneType::kTitle;
    }
}

void GameScene::Draw() {
   // particle_->Draw();
}

SceneType GameScene::GetNextScene() const {
    return nextSceneType_;
}