#include"engine/bonjin/BonjinEngine.h"

//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;


//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	BonjinEngine::Initialize(hInstance, kClientWidth, kClientHeight);
	D3DResourceLeakChecker leakChecker_;

	WorldTransform transform = InitializeWorldTransform();
	Model* model = new Model();
	model->LoadModel("axis");
	model->Initialize(transform);

	Camera* camera = new Camera();
	camera->Initialize(kClientWidth, kClientHeight);

	//ウィンドウの×ボタンが押されるまでループ
	MSG msg{};
	while (msg.message != WM_QUIT) {
		//Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			BonjinEngine::NewFrame();
			///
			/// 更新処理ここから
			///
			
			camera->Update(Camera::CameraType::kDebug);
			model->Update(transform,camera);

			///
			/// 更新処理ここまで
			/// 
			BonjinEngine::PreDraw();
			///
			/// 描画処理ここから

			model->Draw();

			///
			/// 描画処理ここまで
			///
			BonjinEngine::EndFrame();
		}
	}

	/////  解放処理 /////
	delete camera;
	delete model;
	BonjinEngine::Finalize();
	return 0;
}