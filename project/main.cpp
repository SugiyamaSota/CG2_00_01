#include"BonjinEngine.h"

#include"application/scene/manager/SceneManager.h"

using namespace BonjinEngine;

//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakChecker_;
	Initialize();

	SceneManager::GetInstance()->Initialize();
	SceneManager::GetInstance()->AddScene(SceneType::kGame, new GameScene());

	while (true) {
		//Windowにメッセージが来てたら最優先で処理させる
		if (WinApp::GetInstance()->ProcessMessage()) {
			break;
		}
		NewFrame();
		///
		/// 更新処理ここから
		///

		float deltaTime = Time::GetInstance()->GetDeltaTime();

		SceneManager::GetInstance()->Update(deltaTime);


		///
		/// 更新処理ここまで
		/// 
		PreDraw();
		///
		/// 描画処理ここから
		///
		// グリッドとデバッグ用天球
		SceneManager::GetInstance()->Draw();

		///
		/// 描画処理ここまで
		///
		EndFrame();
	}

	/////  解放処理 /////
	SceneManager::DestroyInstance();
	Finalize();
	return 0;
}