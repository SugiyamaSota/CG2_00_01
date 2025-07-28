#pragma once
#include<string>
#include<d3d12.h>
#include <wrl/client.h>
#include"../math/Struct.h"

class DirectXCommon;
class Camera;

class Model {
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="enableLighting">ライティング(デフォルトでfalse)</param>
	Model(bool enableLighting = false);

	void LoadModel(const std::string& fileName);

	void Initialize(WorldTransform worldTransform, Vector4 color);

	void Update(Camera* camera);

	void Draw(); // 引数をintに変更

	WorldTransform& GetTransform() { return transform_; }
	Material& GetMaterialData() { return *materialData_; }

private:
	ModelData modelData_;//構造体

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
	// ビュー行列
	Matrix4x4 viewMatrix_;
	// 射影行列
	Matrix4x4 projectionMatrix_;
	// ビュープロジェクション行列
	Matrix4x4 viewProjectionMatrix_;

	int textureHandle_ = 0;

	bool enableLighting_;

	//mtlファイルの読み込み
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	//objファイルの読み込み
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

};