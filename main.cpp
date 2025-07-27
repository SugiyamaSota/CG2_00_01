#include"engine/bonjin/BonjinEngine.h"
#include"game/GameScene.h"

using namespace BonjinEngine;

//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakChecker_;
	Initialize(hInstance, kClientWidth, kClientHeight);

	GameScene* gameScene = new GameScene();
	gameScene->Initialize(kClientWidth, kClientHeight);
	gameScene->Update(); // Initialize直後のUpdateは不要な場合が多いですが、残しておきます

	bool gameStart = false;

	// Lineクラスのインスタンス化と初期化
	Line lineRenderer;
	// ベジェ曲線が複数の線分で構成されるため、十分な頂点数を確保
	lineRenderer.Initialize(4096); // 例: 1024個の頂点（512線分）*4 = 4096

	// ベジェ曲線パスの制御点を定義
	std::vector<Vector3> bezierPathPoints = {
		{ 0.0f, 0.0f, 0.0f },   // p0
		{ 15.0f, 15.0f, 0.0f }, // p2
		{ 10.0f, 15.0f, 0.0f }, // p3 (最初の曲線の終点、次の曲線の始点)
		{ 20.0f, 15.0f, 0.0f }, // p4
		{ 20.0f, 0.0f, 0.0f },  // p5
		{ 30.0f, 0.0f, 0.0f }   // p6
	};

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

			if (ImGui::Button("start")) {
				gameStart = true;
			}

			if (gameStart) {
				gameScene->Update();
			}

			lineRenderer.Clear();

			lineRenderer.AddBezierPath(bezierPathPoints, 30, Color::Green, *gameScene->GetCamera());

			///
			/// 更新処理ここまで
			/// 
			PreDraw();
			///
			/// 描画処理ここから
			///

			gameScene->Draw();

			lineRenderer.Draw();

			///
			/// 描画処理ここまで
			///
			EndFrame();
		}
	}

	/////  解放処理 ////
	delete gameScene;
	Finalize();
	return 0;
}