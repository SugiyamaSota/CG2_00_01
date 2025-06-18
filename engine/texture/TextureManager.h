#pragma once
#include <d3d12.h>
#include <vector>
#include <string>
#include <wrl/client.h>
#include "../../externals/DirectXTex/DirectXTex.h" // DirectXTexに必要なヘッダー

// 前方宣言
class DirectXCommon; // DirectXCommonクラスの前方宣言

class TextureManager {
public:
    // テクスチャハンドル構造体
    struct TextureHandle {
        uint32_t index; // テクスチャのインデックス
    };

    TextureManager();
    ~TextureManager();

    // 初期化
    void Initialize(DirectXCommon* common, uint32_t srvHeapStartOffset = 0);

    // テクスチャをロードし、ハンドルを返す
    TextureHandle LoadTexture(const std::string& filePath);

    // 指定したテクスチャハンドルのGPUディスクリプタハンドルを取得
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(TextureHandle handle) const;

    // 指定したテクスチャハンドルのメタデータを取得
    const DirectX::TexMetadata& GetMetadata(TextureHandle handle) const;

    // GPUがリソースの使用を完了するまで待機し、中間リソースを解放する関数を追加
    void ReleaseIntermediateResources();

private:
    DirectXCommon* common_ = nullptr; // DirectXCommonへのポインタ

    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> textureResources_; // ロードされたテクスチャリソース
    std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> gpuDescriptorHandles_; // GPUディスクリプタハンドル
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> cpuDescriptorHandles_; // CPUディスクリプタハンドル (デバッグ用)
    std::vector<DirectX::TexMetadata> metadatas_; // テクスチャのメタデータ
    uint32_t nextHandleIndex_ = 0; // マネージャー内部のインデックス
    uint32_t srvHeapBaseOffset_ = 0; // デスクリプタヒープ上の開始オフセット

    // ★追加★
    // アップロード用の中間リソースを保持するリスト。
    // 各要素は、そのリソースがGPUによって完了されるべきフェンス値を持つ
    struct UploadResourceEntry {
        Microsoft::WRL::ComPtr<ID3D12Resource> resource;
        UINT64 fenceValue;
    };
    std::vector<UploadResourceEntry> pendingUploadResources_;



private:
    // 内部的なテクスチャ読み込み処理
    DirectX::ScratchImage LoadTextureInternal(const std::string& filePath);
    // 内部的なテクスチャリソース作成処理
    Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResourceInternal(ID3D12Device* device, const DirectX::TexMetadata& metadata);
    // 内部的なテクスチャデータアップロード処理 (中間リソースを返す)
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureDataInternal(
        ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
};