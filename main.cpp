#include"engine/bonjin/BonjinEngine.h"
#include"GameScene.h"

using namespace BonjinEngine;

//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakChecker_;
	Initialize(hInstance, kClientWidth, kClientHeight);

	GameScene* gameScene = new GameScene();
	gameScene->Initialize();

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
			
			gameScene->Update();

			///
			/// 更新処理ここまで
			/// 
			PreDraw();
			///
			/// 描画処理ここから
			///

			gameScene->Update();

			///
			/// 描画処理ここまで
			///
			EndFrame();
		}
	}

	/////  解放処理 /////
	delete gameScene;
	delete camera;
	Finalize();
	return 0;
}