#include "TitleScene.h"

using namespace BonjinEngine;

void TitleScene::Initialize(Camera* camera) {
    // ���̃V�[���ƑJ�ڌ�V�[��(�����l�͓���)
    currentSceneType_ = SceneType::kTitle;
    nextSceneType_ = SceneType::kTitle;

    this->camera_ = camera;

	blackScreenSprite_ = new Sprite();
	blackScreenSprite_->Initialize({ 640.0f,360.0f,0.0f }, Color::Black, { 0.5f,0.5f,0.0f }, { 1280,720 }, "uvChecker.png");

	// �t�F�[�Y�ƃ^�C�}�[��������
	phase_ = TitlePhase::kFadeIn;
	phaseTimer_ = 0.0f;

	worldTransform_ = InitializeWorldTransform();
	titleModel_ = new Model;
	titleModel_->LoadModel("title");
	titleModel_->Update(worldTransform_, camera_);

	titleUIModel_ = new Model;
	titleUIModel_->LoadModel("titleUI");
	titleUIModel_->Update(worldTransform_, camera_);

	// �V��
	skydomeModel_ = std::make_unique<Model>();
	skydomeModel_->LoadModel("debugSkydome");
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get(), camera_);
}




void TitleScene::Update(float deltaTime) {
    WorldTransform wt = InitializeWorldTransform();
	skydome_->Update();

	titleModel_->Update(worldTransform_, camera_);
	titleUIModel_->Update(worldTransform_, camera_);

	phaseTimer_ += 1.0f / 60.0f;

	switch (phase_) {
	case TitlePhase::kFadeIn:
	{
		// �t�F�[�h�C���̃A���t�@�l���v�Z
		float alpha = 1.0f - min(phaseTimer_ / 2.0f, 1.0f); // 2�b�����ăt�F�[�h�C��
		blackScreenSprite_->Update({ 640.0f,360.0f,0.0f }, Color::Black);
		blackScreenSprite_->SetColor({ 0.0f, 0.0f, 0.0f, alpha });

		// �t�F�[�h�C�������������玟�̃t�F�[�Y��
		if (alpha <= 0.0f) {
			phase_ = TitlePhase::kActive;
			phaseTimer_ = 0.0f;
		}
	}
	break;
	case TitlePhase::kActive:
		// �X�y�[�X�L�[�������ꂽ��t�F�[�h�A�E�g�J�n
		if (Input::GetInstance()->IsPadTrigger(0) || Input::GetInstance()->IsTrigger(DIK_SPACE)) {
			phase_ = TitlePhase::kFadeOut;
			phaseTimer_ = 0.0f;
		}
		break;
	case TitlePhase::kFadeOut:
	{
		// �t�F�[�h�A�E�g�̃A���t�@�l���v�Z
		float alpha = min(phaseTimer_ / 2.0f, 1.0f); // 2�b�����ăt�F�[�h�A�E�g
		blackScreenSprite_->Update({ 640.0f,360.0f,0.0f }, Color::Black);
		blackScreenSprite_->SetColor({ 0.0f, 0.0f, 0.0f, alpha });

		// �t�F�[�h�A�E�g������������V�[���ύX
		if (alpha >= 1.0f) {
			nextSceneType_ = SceneType::kGame;
		}
	}
	break;
	}
}

void TitleScene::Draw() {
	// �V���̕`��
	skydome_->Draw();
	titleModel_->Draw();
	titleUIModel_->Draw();
	// �����X�v���C�g��`��
	blackScreenSprite_->Draw();
}

SceneType TitleScene::GetNextScene() const {
    return nextSceneType_;
}