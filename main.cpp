#include <Windows.h>

// 標準C++ライブラリ
#include <cassert>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <math.h>
#define _USE_MATH_DEFINES

// DirectX関連のSDKヘッダー
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <dbghelp.h>
#include <strsafe.h>
#include <wrl/client.h>

// DirectX関連のライブラリリンク
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"Dbghelp.lib")

// DirectXTex関連の外部ライブラリヘッダー
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

//エンジン
#include"engine/bonjin/BonjinEngine.h"

// 汎用関数
#include"engine/function/Utility.h"

// 音声関連のヘッダー
#include"engine/audio/AudioPlayer.h"

// プロジェクト固有のヘッダー
#include"engine/math/Struct.h"
#include"engine/math/Convert.h"
#include"engine/math/Matrix.h"

// デバッグカメラのヘッダー
#include"engine/debugCamera/DebugCamera.h"

// TextureManagerのヘッダーを追加
#include"engine/texture/TextureManager.h"

#include"engine/model/Model.h"

// 現在時刻を取得
std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
// ログファイルの名前にコンマ何秒はいらないので、削って秒にする
std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
// 日本時間に変換
std::chrono::zoned_time localTime{ std::chrono::current_zone(),nowSeconds };
// formatを使って年月日_時分秒の文字列に変換
std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
// 時刻を使ってファイル名決定
std::string logFilePath = std::string("logs/") + dateString + ".log";
// ファイルを使って書き込み準備
std::ofstream logStream(logFilePath);

//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

BonjinEngine* bonjin = new BonjinEngine();

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
	//リソースリークチェック
	D3DResourceLeakChecker leakChecker_;

	bonjin->Initialize(hInstance, kClientWidth, kClientHeight);

	
	// axisモデル初期化
	WorldTransform axisTransform = {
		{0.7f,0.7f,0.7f},
		{0.0f,0.0f,0.0f},
		{0.5f,0.5f,0.0f},
	};
	Model* axis = new Model(bonjin->GetCommon());
	axis->LoadModel("axis");
	axis->Initialize(axisTransform);

	// cubeモデル初期化
	WorldTransform transform = {
		{0.7f,0.7f,0.7f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	Model* model = new Model(bonjin->GetCommon());
	model->LoadModel("cube");
	model->Initialize(transform);

	///// テクスチャー読み込み /////
	TextureManager* textureManager = new TextureManager();
	textureManager->Initialize(bonjin->GetCommon(), 1);

	TextureManager::TextureHandle uvCheckerTexture = textureManager->LoadTexture("resources/uvChecker.png");
	textureManager->ReleaseIntermediateResources();
	D3D12_GPU_DESCRIPTOR_HANDLE uvCheckerGpuHandle = textureManager->GetGPUHandle(uvCheckerTexture);

	// デバッグカメラの初期化
	DebugCamera debugCamera;
	debugCamera.Initialize(bonjin->GetKey());

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
			debugCamera.Update();
			debugCamera.Check();

			ImGui::Begin("Debug");
			if (ImGui::CollapsingHeader("Camera")) {
				if (ImGui::Button("ResetCameraPosition")) {
					debugCamera.ResetPosition();
				}
				if (ImGui::Button("ResetCameraRotation")) {
					debugCamera.ResetRotation();
				}
			}
			ImGui::End();

			model->Update(transform, &debugCamera);
			axis->Update(axisTransform, &debugCamera);

			///
			/// 更新処理ここまで
			/// 

			///
			/// 描画処理ここから
			///
			// モデル
			model->Draw(uvCheckerGpuHandle);
			axis->Draw(uvCheckerGpuHandle);
			///
			/// 描画処理ここまで
			///
			bonjin->EndFrame();
		}
	}

	/////  解放処理 /////

	delete textureManager;
	delete model;
	delete axis;
	delete bonjin;

	return 0;
}