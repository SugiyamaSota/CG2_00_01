#include"engine/bonjin/BonjinEngine.h"

#include"game/scene/GameScene.h"

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

	Sprite* HUD = new Sprite();
	HUD->Initialize({ 640.0f,360.0f,0.0f }, Color::White, { 0.5f,0.5f,0.0f }, {1280,720}, "HUD.png");

	Sprite* tutrial = new Sprite();
	tutrial->Initialize({ 640.0f,360.0f,0.0f }, Color::White, { 0.5f,0.5f,0.0f }, { 1280,720 }, "tutrial.png");

	bool showTutrial = false;

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

			if (Input::GetInstance()->IsTrigger(DIK_TAB)) {
				if (showTutrial == false) {
					showTutrial = true;
				} else {
					showTutrial = false;
				}
			}

			if (showTutrial == false) {
				HUD->Update({ 640.0f,360.0f,0.0f }, Color::White);
				gameScene->Update();
			} else {
				tutrial->Update({ 640.0f,360.0f,0.0f }, Color::White);
			}


			///
			/// 更新処理ここまで
			/// 
			PreDraw();
			///
			/// 描画処理ここから
			///

			gameScene->Draw();

			if (showTutrial == false) {
				HUD->Draw();
			} else {
				tutrial->Draw();
			}

			///
			/// 描画処理ここまで
			///
			EndFrame();
		}
	}

	/////  解放処理 /////
	delete tutrial;
	delete HUD;
	delete gameScene;
	Finalize();
	return 0;
}