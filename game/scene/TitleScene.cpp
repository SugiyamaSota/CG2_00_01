#include "TitleScene.h"
#include<algorithm>

void TitleScene::Initialize() {
	isFinished_ = false;

	blackScreenSprite_ = new Sprite();
	blackScreenSprite_->Initialize({ 640.0f,360.0f,0.0f }, Color::Black, { 0.5f,0.5f,0.0f }, { 1280,720 }, "uvChecker.png");

	// フェーズとタイマーを初期化
	phase_ = TitlePhase::kFadeIn;
	phaseTimer_ = 0.0f;
}

void TitleScene::Update() {
	phaseTimer_ += 1.0f / 60.0f;

	switch (phase_) {
	case TitlePhase::kFadeIn:
	{
		// フェードインのアルファ値を計算
		float alpha = 1.0f - min(phaseTimer_ / 2.0f, 1.0f); // 2秒かけてフェードイン
		blackScreenSprite_->Update({ 640.0f,360.0f,0.0f }, Color::Black);
		blackScreenSprite_->SetColor({ 0.0f, 0.0f, 0.0f, alpha });

		// フェードインが完了したら次のフェーズへ
		if (alpha <= 0.0f) {
			phase_ = TitlePhase::kActive;
			phaseTimer_ = 0.0f;
		}
	}
	break;
	case TitlePhase::kActive:
		// スペースキーが押されたらフェードアウト開始
		if (Input::GetInstance()->IsTrigger(DIK_SPACE)) {
			phase_ = TitlePhase::kFadeOut;
			phaseTimer_ = 0.0f;
		}
		break;
	case TitlePhase::kFadeOut:
	{
		// フェードアウトのアルファ値を計算
		float alpha =min(phaseTimer_ / 2.0f, 1.0f); // 2秒かけてフェードアウト
		blackScreenSprite_->Update({ 640.0f,360.0f,0.0f }, Color::Black);
		blackScreenSprite_->SetColor({ 0.0f, 0.0f, 0.0f, alpha });

		// フェードアウトが完了したらシーン変更
		if (alpha >= 1.0f) {
			isFinished_ = true;
		}
	}
	break;
	}
}

void TitleScene::Draw() {
	// 黒いスプライトを描画
	blackScreenSprite_->Draw();
}