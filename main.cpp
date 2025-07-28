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

	// カメラ
	Camera* camera = new Camera();
	camera->Initialize(kClientWidth, kClientHeight);

	// 選択中のモデルインデックス
	int selectedModelIndex = 0;

	// モデル数
	int modelNum = 6;

	// モデル名
	std::vector<std::string> modelNames = {
		"monkey",
		"axis",
		"multiMaterial",
		"multiMesh",
		"bunny",
		"teapot",
	};

	// モデルワールドトランスフォーム
	std::vector<WorldTransform> worldTransforms;

	// モデル
	std::vector < Model*> models;

	//モデルそれぞれの初期化
	for (int i = 0; i < modelNum; i++) {
		worldTransforms.push_back(InitializeWorldTransform());

		Model * newModel = new Model();
		newModel->LoadModel(modelNames[i]);
		newModel->Initialize(worldTransforms[i],{1,1,1,1});

		models.push_back(newModel);

		ImGuiManager::GetInstance()->RegisterModel(models[i], modelNames[i]);
	}

	WorldTransform spriteWorldTransform = InitializeWorldTransform();
	Sprite* sprite = new Sprite();
	sprite->Initialize(spriteWorldTransform, "uvChecker.png");

	// Gridクラスのインスタンスを生成し、初期化
	Grid* grid = new Grid();
	grid->Initialize();

	WorldTransform skydomeWorldTransform = InitializeWorldTransform();
	Model* skydome = new Model();
	skydome->LoadModel("debugSkydome");
	skydome->Initialize(skydomeWorldTransform,{ 0.05f,0.05f,0.05f,1.0f });

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

			ImGuiManager::GetInstance()->DrawCombinedModelDebugUI(selectedModelIndex);

			for (int i = 0; i < modelNum; i++) {
				models[i]->Update(camera); // cameraとcolorは適宜調整
			}

			sprite->Update(spriteWorldTransform, Color::White);

			// グリッドとデバッグ用天球の更新
			skydome->Update(camera);
			grid->Update(camera);


			///
			/// 更新処理ここまで
			/// 
			PreDraw();
			///
			/// 描画処理ここから
			///

			if (selectedModelIndex >= 0 && selectedModelIndex < modelNum) {
				models[selectedModelIndex]->Draw(); // 選択されたモデルのみ描画
			}

			sprite->Draw();

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
	delete sprite;
	for (int i = 0; i < modelNum; i++) {
		delete models[i];
	}
	delete camera;
	Finalize();
	return 0;
}