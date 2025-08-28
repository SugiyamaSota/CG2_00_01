#include "SceneManager.h"

void SceneManager::Initialize() {
	switch (nowScene) {
	case Scene::kTitle:
		titleScene_ = new TitleScene;
		titleScene_->Initialize();
		break;
	case Scene::kGame:
		gameScene_ = new GameScene;
		gameScene_->Initialize();
		break;
	}
}

void SceneManager::Update() {
	switch (nowScene) {
	case Scene::kTitle:
		titleScene_->Update();
		if (titleScene_->GetIsFinished()) {
			ChangeScene();
		}
		break;
	case Scene::kGame:
		gameScene_->Update();
		if (gameScene_->GetSceneChengeStandby()) {
			ChangeScene();
		}
		break;
	}
}

void SceneManager::Draw() {
	switch (nowScene) {
	case Scene::kTitle:
		titleScene_->Draw();
		break;
	case Scene::kGame:
		gameScene_->Draw();
		break;
	}
}

void SceneManager::ChangeScene() {
	switch (nowScene) {
	case Scene::kTitle:
		nowScene = Scene::kGame;
		Initialize();
		break;
	case Scene::kGame:
		nowScene = Scene::kTitle;
		Initialize();
		break;
	}
}
