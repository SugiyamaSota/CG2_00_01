#include "Fade.h"
#define NOMINMAX
#include<algorithm>

void Fade::Initialize() {
	transformSprite_ = {
			{2.0f,4.0f,1.0f},
			{0.0f,0.0f,0.0f},
			{0.0f,0.0f,0.0f},
	};

	viewMatrixSprite_ = MakeIdentity4x4();
	projectionMatrixSprite_ = MakeOrthographicMatrix(0.0f, 0.0f, float(1280), float(720), 0.0f, 100.0f);
	sprite_ = new Sprite();
	sprite_->Initialize(transformSprite_, "resources/uvChecker.png");
	sprite_->Update(transformSprite_, { 0,0,0,1 }, viewMatrixSprite_, projectionMatrixSprite_);
}

Fade::~Fade() {
	delete sprite_;
}

void Fade::Update() {
	switch (status_) {
	case Status::None:
		//何もない
		break;
	case Status::FadeIn:
		// 1フレームの秒数をカウントアップ
		counter_ += 1.0f / 60.0f;
		// フェード継続時間に達したら打ち止め
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		sprite_->Update(transformSprite_, Vector4(0, 0, 0, std::clamp(1.0f - (counter_ / duration_), 0.0f, 1.0f)), viewMatrixSprite_, projectionMatrixSprite_);
		break;
	case Status::FadeOut:
		counter_ += 1.0f / 60.0f;
		if (counter_ >= duration_) {
			counter_ = duration_;
		}
		sprite_->Update(transformSprite_, Vector4(0, 0, 0, std::clamp(counter_ / duration_, 0.0f, 1.0f)), viewMatrixSprite_, projectionMatrixSprite_);
		break;
	}
}

void Fade::Draw() {
	if (status_ == Status::None) {
		return;
	}
	sprite_->Draw();
}

void Fade::Start(Status status, float duration) {
	status_ = status;
	duration_ = duration;
	counter_ = 0.0f;
}

void Fade::Stop() {
	status_ = Status::None;
}

bool Fade::IsFinished()const {
	switch (status_) {
	case Status::FadeIn:
	case Status::FadeOut:
		if (counter_ >= duration_) {
			return true;
		} else {
			return false;
		}
	}
	return true;
}