#include"IScene.h"

using namespace BonjinEngine;

void IScene::Initialize(Camera* camera) {
	assert(camera);
	camera_ = camera;
}

void IScene::Unload() {

}

void IScene::DrawImGui() {
	ImGui::Begin(GetScenename());

	DrawSceneImGui();

	ImGui::End();
}

