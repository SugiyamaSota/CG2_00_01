#include"engine/bonjin/BonjinEngine.h"

#include"game/scene/SceneManager.h"

using namespace BonjinEngine;

//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakChecker_;
	Initialize(hInstance, kClientWidth, kClientHeight);

	SceneManager* sceneManager_ = new SceneManager();
	sceneManager_->Initialize();

	//ウィンドウの×ボタンが押されるまでループ
	MSG msg{};
	while (msg.message != WM_QUIT) {
		//Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			NewFrame();
			///
			/// 更新処理ここから
			///

			sceneManager_->Update();

			///
			/// 更新処理ここまで
			/// 
			PreDraw();
			///
			/// 描画処理ここから
			///

			sceneManager_->Draw();

			///
			/// 描画処理ここまで
			///
			EndFrame();
		}
	}

	/////  解放処理 /////
	delete sceneManager_;
	Finalize();
	return 0;
}