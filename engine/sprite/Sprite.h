#pragma once
#include"../bonjin/BonjinEngine.h"

class Sprite {
public:
    Sprite();
    ~Sprite(); // リソースを解放するためのデストラクタ

    /// <summary>
    /// スプライトの初期化
    /// </summary>
    /// <param name="worldTransform">ワールド変換</param>
    /// <param name="textureFilePath">テクスチャファイルのパス</param>
    void Initialize(WorldTransform worldTransform, const std::string& textureFilePath);

    /// <summary>
    /// スプライトの更新
    /// </summary>
    /// <param name="worldTransform">ワールド変換</param>
    /// <param name="color">色</param>
    /// <param name="viewMatrix">ビュー行列</param>
    /// <param name="projectionMatrix">プロジェクション行列</param>
    void Update(WorldTransform worldTransform, Vector4 color, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix);

    /// <summary>
    /// スプライトの描画
    /// </summary>
    void Draw();

private:
    // 頂点リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
    VertexData* vertexData_ = nullptr;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

    // インデックスリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;
    uint32_t* indexData_ = nullptr;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

    // マテリアルリソース
    Material* materialData_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;

    // WVP リソース
    TransformationMatrix* wvpData_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_ = nullptr;

    WorldTransform transform_;
    int textureHandle_ = 0;
};