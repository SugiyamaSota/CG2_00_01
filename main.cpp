#include"BonjinEngine.h"

using namespace BonjinEngine;

//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakChecker_;
	Initialize();

	Camera* camera = new Camera();
	camera->Initialize(kClientWidth, kClientHeight);

	// モデル
	WorldTransform worldTransform = InitializeWorldTransform();
	Model* model = new Model();
	model->LoadModel("fence");

	// グリッド
	Grid* grid = new Grid();
	grid->Initialize(); // グリッドの初期化 (デフォルトのサイズと分割数)

	// 天球
	WorldTransform skydomeWorldTransform = InitializeWorldTransform();
	Model* skydome = new Model();
	skydome->LoadModel("debugSkydome");

	while (true) {
		//Windowにメッセージが来てたら最優先で処理させる
		if (WinApp::GetInstance()->ProcessMessage()) {
			break;
		}
		NewFrame();
		///
		/// 更新処理ここから
		///

		camera->Update(Camera::CameraType::kDebug);

		model->SetEnableLighting(true);
		model->Update(worldTransform, camera);

		// グリッドとデバッグ用天球
		skydome->Update(skydomeWorldTransform, camera);
		grid->Update(camera);


		///
		/// 更新処理ここまで
		/// 
		PreDraw();
		///
		/// 描画処理ここから
		///
		// グリッドとデバッグ用天球
		skydome->Draw();
		grid->Draw();

		model->Draw();

		///
		/// 描画処理ここまで
		///
		EndFrame();
	}

	/////  解放処理 /////
	delete skydome;
	delete grid;
	delete model;
	delete camera;
	Finalize();
	return 0;
}