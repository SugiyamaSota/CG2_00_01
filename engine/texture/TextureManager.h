#pragma once
#include <d3d12.h>
#include <vector>
#include <string>
#include <wrl/client.h>
#include "../../externals/DirectXTex/DirectXTex.h"
#include<map>

class TextureManager {
public:
    // シングルトンインスタンスへのアクセスを提供する静的メソッド
    static TextureManager* GetInstance();
    static void DestroyInstance();

    // コピーコンストラクタと代入演算子を削除し、コピーを禁止します
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    // デストラクタ
    ~TextureManager();

    // 初期化 (GetInstanceから呼び出される)
    void Initialize();

    // テクスチャをロードし、そのインデックス（int）を返す
    int LoadTexture(const std::string& filePath); // 戻り値をintに変更

    // 指定したインデックスのGPUディスクリプタハンドルを取得
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(int textureIndex) const; // 引数をintに変更

    // 指定したインデックスのメタデータを取得
    const DirectX::TexMetadata& GetMetadata(int textureIndex) const; // 引数をintに変更

    // GPUがリソースの使用を完了するまで待機し、中間リソースを解放する関数を追加
    void ReleaseIntermediateResources();

private:
    // プライベートコンストラクタ (シングルトンパターン用)
    TextureManager();

    // シングルトンインスタンスを保持する静的メンバー変数
    static TextureManager* instance_;

    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> textureResources_; // ロードされたテクスチャリソース
    std::vector<D3D12_GPU_DESCRIPTOR_HANDLE> gpuDescriptorHandles_; // GPUディスクリプタハンドル
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> cpuDescriptorHandles_; // CPUディスクリプタハンドル (デバッグ用)
    std::vector<DirectX::TexMetadata> metadatas_; // テクスチャのメタデータ
    uint32_t nextHandleIndex_ = 0; // マネージャー内部のインデックス
    uint32_t srvHeapBaseOffset_ = 0; // デスクリプタヒープ上の開始オフセット

    std::map<std::string, int> loadedTextures_;

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