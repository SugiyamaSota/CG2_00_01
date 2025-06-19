#pragma once
#include<string>
#include<d3d12.h>
#include <wrl/client.h>
#include"../math/Struct.h"

class DirectXCommon;
class DebugCamera;

class Model {
public:
	Model(DirectXCommon* common);

	void LoadModel(const std::string& fileName);

	void Initialize(WorldTransform worldTransform);

	void Update(WorldTransform worldTransform, DebugCamera* debugCamera);

	void Draw(D3D12_GPU_DESCRIPTOR_HANDLE handle);

private:
	DirectXCommon* common_ = nullptr;
	ModelData modelData_;

	// 頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	VertexData* vertexData_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	// マテリアルリソース
	Material* materialData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_ = nullptr;
	TransformationMatrix* wvpData_ = nullptr;

	WorldTransform transform_;
	Matrix4x4 projectionMatrix_;

	//mtlファイルの読み込み
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	//objファイルの読み込み
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);
};

