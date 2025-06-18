#include "TextureManager.h"
#include <cassert>
#include "../function/Utility.h" // For ConvertString, CreateBufferResource, GetCPUDescriptorHandle, GetGPUDescriptorHandle
#include "../common/DirectXCommon.h" // Getters for device, commandList, etc.
#include "../../externals/DirectXTex/d3dx12.h" // GetRequiredIntermediateSize, UpdateSubresources のために必要

TextureManager::TextureManager() = default;

TextureManager::~TextureManager() {
    // 終了時に残っている中間リソースを解放
    ReleaseIntermediateResources();
}

void TextureManager::Initialize(DirectXCommon* common, uint32_t srvHeapStartOffset) {
    assert(common);
    common_ = common;
    srvHeapBaseOffset_ = srvHeapStartOffset; // オフセットを設定
    nextHandleIndex_ = 0; // 内部インデックスは0から始める
}

TextureManager::TextureHandle TextureManager::LoadTexture(const std::string& filePath) {
    assert(common_);

    // 内部的なテクスチャ読み込み
    DirectX::ScratchImage mipImages = LoadTextureInternal(filePath);
    const DirectX::TexMetadata& metadata = mipImages.GetMetadata();

    // テクスチャリソースの作成
    Microsoft::WRL::ComPtr<ID3D12Resource> textureResource = CreateTextureResourceInternal(common_->GetDevice(), metadata);

    // テクスチャデータのアップロード
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = UploadTextureDataInternal(
        textureResource.Get(), mipImages, common_->GetDevice(), common_->GetCommandList());

    // ★追加★
    // アップロードコマンドをキューに投入する前に、中間リソースを保留リストに追加
    // この時点でまだコマンドリストはクローズされていないが、このコマンドリストが実行される際のフェンス値を関連付ける
    // コマンドリストはcommon->WaitAndResetCommandList()で実行されるため、その際のフェンス値を取得して紐づける
    pendingUploadResources_.push_back({ intermediateResource, common_->GetFenceValue() + 1 }); // +1 はSignalがこれから行われるため

    // SRVディスクリプタヒープから新しいディスクリプタハンドルを取得
    // Utility関数のGetCPUDescriptorHandleとGetGPUDescriptorHandleを使用
    ID3D12DescriptorHeap* srvHeap = common_->GetSRVDescriptorHeap();
    UINT srvDescriptorSize = common_->GetSRVSize(); // DirectXCommonからサイズを取得

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = GetCPUDescriptorHandle(srvHeap, srvDescriptorSize, srvHeapBaseOffset_ + nextHandleIndex_);
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = GetGPUDescriptorHandle(srvHeap, srvDescriptorSize, srvHeapBaseOffset_ + nextHandleIndex_);

    // 全てのテクスチャロードが終わった後で、まとめてコマンドリストを実行し、完了を待機する
     // commandListをcloseし、キックする
    HRESULT hr = common_->GetCommandList()->Close();
    assert(SUCCEEDED(hr));
    ID3D12CommandList* commandLists[] = { common_->GetCommandList() };
    common_->GetCommandQueue()->ExecuteCommandLists(1, commandLists);
    // Fenceの値をインクリメント
    UINT64 currentFenceValue = common_->GetFenceValue();
    common_->IncrementFencevalue();
    // コマンドキューにFenceのシグナルを送る
    hr = common_->GetCommandQueue()->Signal(common_->GetFence(), common_->GetFenceValue());
    assert(SUCCEEDED(hr));
    if (common_->GetFence()->GetCompletedValue() < currentFenceValue) {
        // 指定したsignalにたどり着いてないので、たどり着くまで待つようにイベントを設定する
        common_->GetFence()->SetEventOnCompletion(currentFenceValue, common_->GetFenceEvent());
        // イベントを待つ
        WaitForSingleObject(common_->GetFenceEvent(), INFINITE);
    }
    // 次のフレーム用のコマンドリストを準備
    hr = common_->GetCommandAllocator()->Reset();
    assert(SUCCEEDED(hr));
    hr = common_->GetCommandList()->Reset(common_->GetCommandAllocator(), nullptr);
    assert(SUCCEEDED(hr));
   

    // SRVの設定
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
    srvDesc.Format = metadata.format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

    // SRVの作成
    common_->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, cpuHandle);

    // データを保存
    textureResources_.push_back(textureResource);
    gpuDescriptorHandles_.push_back(gpuHandle);
    cpuDescriptorHandles_.push_back(cpuHandle); // CPUハンドルも保持しておくとデバッグに便利
    metadatas_.push_back(metadata);

    // Handleを生成して返す
    TextureHandle newHandle;
    newHandle.index = nextHandleIndex_;
    nextHandleIndex_++;

    return newHandle;
}

void TextureManager::ReleaseIntermediateResources() {
    // 現在のGPU完了フェンス値を取得
    UINT64 completedFenceValue = common_->GetFence()->GetCompletedValue();

    // ★修正されたループ★
    // 通常のイテレータで前から順に処理する
    for (auto it = pendingUploadResources_.begin(); it != pendingUploadResources_.end(); /*ループ内でitを更新*/) {
        if (it->fenceValue <= completedFenceValue) {
            // このリソースはGPUが使用を完了したため、解放できる
            it->resource.Reset(); // ComPtrが参照カウントを0にする
            // vectorから要素を削除。eraseは削除後の次の要素へのイテレータを返すので、それを使う
            it = pendingUploadResources_.erase(it);
        } else {
            // このリソースはまだ完了していないので、次の要素へ進む
            ++it;
        }
    }
}


D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUHandle(TextureHandle handle) const {
    assert(handle.index < gpuDescriptorHandles_.size());
    return gpuDescriptorHandles_[handle.index];
}

const DirectX::TexMetadata& TextureManager::GetMetadata(TextureHandle handle) const {
    assert(handle.index < metadatas_.size());
    return metadatas_[handle.index];
}

DirectX::ScratchImage TextureManager::LoadTextureInternal(const std::string& filePath) {
    DirectX::ScratchImage image = {};
    std::wstring filePathW = ConvertString(filePath);
    HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
    assert(SUCCEEDED(hr));

    DirectX::ScratchImage mipImages = {};
    hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
    assert(SUCCEEDED(hr));

    return mipImages;
}

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::CreateTextureResourceInternal(ID3D12Device* device, const DirectX::TexMetadata& metadata) {
    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Width = UINT(metadata.width);
    resourceDesc.Height = UINT(metadata.height);
    resourceDesc.MipLevels = UINT(metadata.mipLevels);
    resourceDesc.DepthOrArraySize = UINT(metadata.arraySize);
    resourceDesc.Format = metadata.format;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);

    D3D12_HEAP_PROPERTIES heapProperties = {};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

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

Microsoft::WRL::ComPtr<ID3D12Resource> TextureManager::UploadTextureDataInternal(ID3D12Resource* texture, const DirectX::ScratchImage& mipImages, ID3D12Device* device, ID3D12GraphicsCommandList* commandList) {
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    DirectX::PrepareUpload(device, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
    uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
    Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource = CreateBufferResource(device, intermediateSize);
    UpdateSubresources(commandList, texture, intermediateResource.Get(), 0, 0, UINT(subresources.size()), subresources.data());

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