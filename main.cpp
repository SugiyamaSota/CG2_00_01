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

// DXCommon
#include"engine/common/DirectXCommon.h"

// PSO
#include"engine/pso/PSO.h"

// 汎用関数
#include"engine/function/Utility.h"

// 音声関連のヘッダー
#include"engine/audio/AudioPlayer.h"

// プロジェクト固有のヘッダー
#include"StructCommon.h"
#include "math/Convert.h"
#include "math/Matrix.h"

// 入力関連のヘッダー
#include "engine/input/InputKey.h"

// デバッグカメラのヘッダー
#include"debugCamera/DebugCamera.h"

// ImGui関連のヘッダー
#include"engine/imgui/ImGuiManager.h"

// TextureManagerのヘッダーを追加
#include"engine/texture/TextureManager.h"

#include"engine/model/Model.h"

// 構造体



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

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		//リソースリークチェック
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};


DirectX::ScratchImage LoadTexture(const std::string& filePath) {
	//テクスチャファイルを呼んでプログラムで扱えるようにする
	DirectX::ScratchImage image = {};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミップマップの作成
	DirectX::ScratchImage mipImages = {};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//ミップマップ付きデータを返す
	return mipImages;
}


Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
	//metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc = {};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties = {};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//Resourceの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(resource.GetAddressOf()));
	assert(SUCCEEDED(hr));

	return resource;
}


[[nodiscard]]
Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device,
	ID3D12GraphicsCommandList* commandList) {
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
	UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());
	//Textureへの転送後利用できるようにResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier = {};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
	return intermediateResource;
}

//mtlファイルの読み込み
MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + '/' + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilepath = directoryPath + '/' + textureFilename;
		}
	}
	return materialData;
}

//objファイルの読み込み
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData;
	std::vector<Vector4>positions;
	std::vector<Vector3>normals;
	std::vector<Vector2>texcoords;
	std::string line;

	std::ifstream file(directoryPath + '/' + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {
			VertexData triangle[3];
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position,texcoord,normal };
			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			std::string materialFilename;
			s >> materialFilename;

			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}

//クライアント領域のサイズ
const int32_t kClientWidth = 1280;
const int32_t kClientHeight = 720;

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int) {
	D3DResourceLeakChecker leakCheck;

	DirectXCommon* common = new DirectXCommon(hInstance, kClientWidth, kClientHeight);

	MSG msg{};


	//球のトランスフォーム変数
	WorldTransform transform = {
		{0.7f,0.7f,0.7f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	//球のトランスフォーム変数
	WorldTransform axisTransform = {
		{0.7f,0.7f,0.7f},
		{0.0f,0.0f,0.0f},
		{0.5f,0.5f,0.0f},
	};


	Model* axis = new Model(common);
	axis->LoadModel("resources", "axis.obj");
	axis->Initialize(axisTransform);

	Model* model = new Model(common);
	model->LoadModel("resources/cube", "cube.obj");
	model->Initialize(transform);


	//カメラのトランスフォーム変数
	WorldTransform cameraTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,-5.0f},
	};

	///// テクスチャー読み込み /////
	TextureManager* textureManager = new TextureManager();
	textureManager->Initialize(common, 1);

	TextureManager::TextureHandle uvCheckerTexture = textureManager->LoadTexture("resources/uvChecker.png");
	textureManager->ReleaseIntermediateResources();
	D3D12_GPU_DESCRIPTOR_HANDLE uvCheckerGpuHandle = textureManager->GetGPUHandle(uvCheckerTexture);


	// オーディオの初期化と再生
	AudioPlayer* audioPlayer = new AudioPlayer();
	audioPlayer->Initialize(L"resources/loop1.mp3");

	// デバッグカメラの初期化
	DebugCamera* debugCamera = new DebugCamera();
	debugCamera->Initialize(&common->inputKey);

	//ウィンドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			///
			/// 更新処理ここから
			/// 
			// デバッグカメラ
			debugCamera->Check();
			common->NewFeame();

			ImGui::Begin("Debug");
			if (ImGui::CollapsingHeader("Camera")) {
				if (ImGui::Button("ResetCameraPosition")) {
					debugCamera->ResetPosition();
				}
				if (ImGui::Button("ResetCameraRotation")) {
					debugCamera->ResetRotation();
				}
			}
			ImGui::End();

			model->Update(transform, debugCamera);
			axis->Update(axisTransform, debugCamera);

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
			debugCamera->Update();

			common->EndFrame();
		}
	}

	/////  解放処理 /////

	delete textureManager;
	CloseHandle(common->GetFenceEvent());
	CoUninitialize();
	CloseWindow(common->GetHWND());
	delete common;
	delete model;
	delete axis;

	return 0;
}