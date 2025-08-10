#include "TitleScene.h"

void TitleScene::Initialize() {
	isFinished_ = false;
}

void TitleScene::Update() {
	if (Input::GetInstance()->IsTrigger(DIK_SPACE)) {
		isFinished_ = true;
	}
}

void TitleScene::Draw() {

}