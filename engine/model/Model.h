#pragma once
#include<string>
#include<d3d12.h>
#include <wrl/client.h>
#include <filesystem>
#include <fstream>
#include<cassert>

#include"../math/Struct.h"
#include"../common/DirectXCommon.h"
#include"../texture/TextureManager.h"
#include"../camera/Camera.h"
#include"../function/Utility.h"
#include"../math/Convert.h"
#include"../math/Matrix.h"

class DirectXCommon;
class Camera;

class Model {
public:
	/// --- 汎用関数 ---
	/// <summary>
	///  コンストラクタ
	/// </summary>
	Model();

	/// <summary>
	/// モデルをロード
	/// </summary>
	/// <param name="fileName">モデル名</param>
	void LoadModel(const std::string& fileName);

	/// <summary>
	/// 更新処理
	/// </summary>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="camera">カメラ</param>
	void Update(WorldTransform worldTransform, Camera* camera);

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();


	/// --- 設定関数 ---
	/// <summary>
	/// ライティングの有無
	/// </summary>
	/// <param name="enableLighting">フラグ</param>
	void SetEnableLighting(bool enableLighting) { materialData_->enableLighting = enableLighting; }

	/// <summary>
	/// 色と透明度
	/// </summary>
	/// <param name="color">設定したい色と透明度</param>
	void SetColor(Vector4 color) { materialData_->color = color; }

	/// --- 取得関数 ---
	float GetAlpha() { return materialData_->color.w; }

private:
	/// --- 変数 ---
	// モデルデータ
	ModelData modelData_;

	// 頂点リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	VertexData* vertexData_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	// マテリアルリソース
	Material* materialData_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

	// WVPリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_ = nullptr;
	TransformationMatrix* wvpData_ = nullptr;

	// トランスフォーム
	WorldTransform transform_;

	// ビュー行列
	Matrix4x4 viewMatrix_;
	// 射影行列
	Matrix4x4 projectionMatrix_;
	// ビュープロジェクション行列
	Matrix4x4 viewProjectionMatrix_;

	// テクスチャハンドル
	int textureHandle_ = 0;

	// dxcommonのインスタンス
	DirectXCommon* common = nullptr;

	/// --- 関数 ---
	/// <summary>
	/// objファイルの読み込み
	/// </summary>
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// mtlファイルの読み込み
	/// </summary>
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

};