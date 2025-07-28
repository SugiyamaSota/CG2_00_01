#include"engine/bonjin/BonjinEngine.h"

using namespace BonjinEngine;

// グリッド描画用の頂点構造体
struct GridVertex {
	Vector4 position;
};

// ワールドビュープロジェクション行列用の定数バッファ
struct GridTransformationMatrix {
	Matrix4x4 worldviewProjection;
};

//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakChecker_;
	Initialize(hInstance, kClientWidth, kClientHeight);

	Camera* camera = new Camera();
	camera->Initialize(kClientWidth, kClientHeight);

	int modelNum = 2;

	std::vector<std::string> modelNames = {
		"bunny",
		"multiMaterial",
		"multiMesh",
	};
	std::vector<WorldTransform> worldTransforms;
	std::vector < Model*> models;

	for (int i = 0; i < modelNum; i++) {
		worldTransforms.push_back(InitializeWorldTransform()); // 要素を追加

		Model* newModel = new Model(); // モデルをnewで生成
		newModel->LoadModel(modelNames[i]); // ファイル名を使ってロード

		models.push_back(newModel); // モデルポインタをvectorに追加
	}

	// Gridクラスのインスタンスを生成し、初期化
	Grid* grid = new Grid();
	grid->Initialize();

	WorldTransform skydomeWorldTransform = InitializeWorldTransform();
	Model* skydome = new Model();
	skydome->LoadModel("debugSkydome");
	skydome->Initialize(skydomeWorldTransform);

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

			camera->Update(Camera::CameraType::kDebug);

			for (int i = 0; i < modelNum; i++) {
				models[i]->Update(worldTransforms[i], camera, {1,1,1,1});
			}

			// グリッドとデバッグ用天球の更新
			skydome->Update(skydomeWorldTransform, camera, { 0.05f,0.05f,0.05f,1.0f });
			grid->Update(camera);


			///
			/// 更新処理ここまで
			/// 
			PreDraw();
			///
			/// 描画処理ここから
			///

			for (int i = 0; i < modelNum; i++) {
				models[i]->Draw();
			}

			// グリッドとデバッグ用天球の描画
			skydome->Draw();
			grid->Draw();


			///
			/// 描画処理ここまで
			///
			EndFrame();
		}
	}

	/////  解放処理 /////
	delete skydome;
	delete grid;
	for (int i = 0; i < modelNum; i++) {
		delete models[i];
	}
	delete camera;
	Finalize();
	return 0;
}