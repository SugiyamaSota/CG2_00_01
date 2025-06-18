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
#include "math/Struct.h"
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

// 構造体
struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float pedding[3];
	Matrix4x4 uvTransform;
};

struct TransformationMatrix {
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct MaterialData {
	std::string textureFilepath;
};

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
};

struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};


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

	///// モデルを描画する準備 /////
	// モデル読み込み
	ModelData modelData = LoadObjFile("resources/cube", "cube.obj");
	// 頂点用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource = CreateBufferResource(common->GetDevice(), sizeof(VertexData) * modelData.vertices.size());
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView.StrideInBytes = sizeof(VertexData);
	VertexData* vertexData = nullptr;
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());

	// マテリアル用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource = CreateBufferResource(common->GetDevice(), sizeof(Material));
	Material* materialData = nullptr;
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = true;
	materialData->uvTransform = MakeIdentity4x4();

	// WVP用のリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource = CreateBufferResource(common->GetDevice(), sizeof(TransformationMatrix));
	TransformationMatrix* wvpData = nullptr;
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	wvpData->WVP = MakeIdentity4x4();
	wvpData->World = MakeIdentity4x4();

	//球のトランスフォーム変数
	Transform transform = {
		{0.7f,0.7f,0.7f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	Transform transformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};

	Transform uvTransformSprite{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f},
	};
	//カメラのトランスフォーム変数
	Transform cameraTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,-5.0f},
	};

	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(kClientWidth) / float(kClientHeight), 0.1f, 100.0f);

	///// テクスチャー読み込み /////
	TextureManager* textureManager = new TextureManager();
	textureManager->Initialize(common,1);

	TextureManager::TextureHandle uvCheckerTexture = textureManager->LoadTexture("resources/uvChecker.png");
	textureManager->ReleaseIntermediateResources();
	D3D12_GPU_DESCRIPTOR_HANDLE uvCheckerGpuHandle = textureManager->GetGPUHandle(uvCheckerTexture);

	TextureManager::TextureHandle modelTexture = textureManager->LoadTexture(modelData.material.textureFilepath);
	textureManager->ReleaseIntermediateResources();
	D3D12_GPU_DESCRIPTOR_HANDLE modelTextureGpuHandle = textureManager->GetGPUHandle(modelTexture);

	int nowBallTexture = 0;
	int nowTriangleTexture = 0;

	// オーディオの初期化と再生
	AudioPlayer* audioPlayer = new AudioPlayer();
	audioPlayer->Initialize(L"resources/loop1.mp3");

	// デバッグカメラの初期化
	DebugCamera* debugCamera = new DebugCamera();
	debugCamera->Initialize(&common->inputKey);

	// ものに注目させるか
	bool isTarget = true;

	//ウィンドウの×ボタンが押されるまでループ
	while (msg.message != WM_QUIT) {
		//Windowにメッセージが来てたら最優先で処理させる
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		} else {
			
			// デバッグカメラ
			debugCamera->Check();

			common->NewFeame();

			ImGui::Begin("Debug");
			if (ImGui::CollapsingHeader("Model")) {
				ImGui::ColorEdit4("color", &materialData->color.x, 0);
				ImGui::DragFloat3("scale", &transform.scale.x, 0.01f);
				ImGui::DragFloat3("rotate", &transform.rotate.x, 0.01f);
				ImGui::DragFloat3("translate", &transform.translate.x, 0.01f);
				ImGui::SliderInt("texture", &nowBallTexture, 0, 1);
				ImGui::Checkbox("target", &isTarget);
			}
			if (ImGui::CollapsingHeader("Sprite")) {
				ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
				ImGui::DragFloat2("UVScale", &uvTransformSprite.scale.x, 0.01f, -10.0f, 10.0f);
				ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
			}
			if (ImGui::CollapsingHeader("Camera")) {
				if (ImGui::Button("ResetCameraPosition")) {
					debugCamera->ResetPosition();
				}
				if (ImGui::Button("ResetCameraRotation")) {
					debugCamera->ResetRotation();
				}
			}
			ImGui::End();

			//ゲームの処理
			// モデル
			wvpData->World = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			Matrix4x4 worldViewProjectionMatrix = Multiply(wvpData->World, Multiply(debugCamera->GetViewMatrix(), projectionMatrix));
			wvpData->WVP = worldViewProjectionMatrix;

			//モデルの描画
			common->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
			//形状を設定。PSOに設定しているものとは別。同じものを設定るすると考える
			common->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//マテリアルCBufferの場所を設定
			common->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			//wvp用のCBufferの場所を設定
			common->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
			//SRVようのdescriptionTavleの先頭を設定
			common->GetCommandList()->SetGraphicsRootDescriptorTable(2, modelTextureGpuHandle);
			//描画!3頂点で1つのインスタンス
			common->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

			// デバッグカメラ
			if (isTarget) {
				debugCamera->SetTarget(transform.translate);
			}
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

	return 0;
}