#include"engine/bonjin/BonjinEngine.h"

#include"TitleScene.h"
#include"GameScene.h"

//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

GameScene* gameScene = new GameScene();
TitleScene* titleScene = new TitleScene();

enum class Scene {
	kUnknow = 0,

	kTitle,
	kGame,
};

Scene scene = Scene::kUnknow;

void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			scene = Scene::kGame;
			delete titleScene;
			titleScene = nullptr;
			gameScene = new GameScene();
			gameScene->Initialize();
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			scene = Scene::kTitle;
			delete gameScene;
			gameScene = nullptr;
			titleScene = new TitleScene();
			titleScene->Initialize();
		}
		break;
	}
}

void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}
}

void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
}


//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	scene = Scene::kTitle;

	BonjinEngine* bonjin = new BonjinEngine();
	bonjin->Initialize(hInstance, kClientWidth, kClientHeight);

	// タイトルシーンのインスタンス生成と初期化
	titleScene->Initialize();

	//ウィンドウの×ボタンが押されるまでループ
	MSG msg{};
	while (msg.message != WM_QUIT) {
		//Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			bonjin->NewFrame();
			///
			/// 更新処理ここから
			///
			
			ChangeScene();

			UpdateScene();

			///
			/// 更新処理ここまで
			/// 
			bonjin->PreDraw();
			///
			/// 描画処理ここから
			///

			DrawScene();

			///
			/// 描画処理ここまで
			///
			bonjin->EndFrame();
		}
	}

	/////  解放処理 /////
	delete gameScene;
	delete titleScene;
	delete bonjin;
	return 0;
}