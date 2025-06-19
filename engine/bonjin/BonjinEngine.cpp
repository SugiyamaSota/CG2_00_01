#include "BonjinEngine.h"

void BonjinEngine::Initialize(HINSTANCE hInstance, int32_t clientWidth, int32_t clientHeight) {
	// directXcommonの初期化
	common_ = new DirectXCommon(hInstance, clientWidth, clientHeight);
	key_ = new InputKey();
	key_->Initialize(hInstance, common_->GetHWND());
}

BonjinEngine::~BonjinEngine() {
	delete common_;
}

void BonjinEngine::NewFrame() {
	common_->NewFeame();
	key_->Update();
}

void BonjinEngine::EndFrame() {
	common_->EndFrame();
}
