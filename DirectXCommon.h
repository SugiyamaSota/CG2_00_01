#pragma once
#include<stdint.h>

// DirectX関連のSDKヘッダー
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <dbghelp.h>
#include <strsafe.h>
#include <wrl/client.h>

class DirectXCommon {
public:
	// コンストラクタデストラクタ
	DirectXCommon(HWND hwnd, int32_t kClientWidth, int32_t ClientHeight);
	~DirectXCommon();

	// 初期化
	void Initialize();

	// ゲッター
	ID3D12Device* GetDevice() { return device_.Get(); }                                // デバイス
	IDXGIFactory7* GetDxgiFactory() { return dxgiFactory_.Get(); }                     // dxgiファクトリー

	ID3D12CommandQueue* GetCommandQueue() { return commandQueue_.Get(); }              // コマンドキュー
	ID3D12CommandAllocator* GetCommandAllocator() { return commandAllocator_.Get(); }  // コマンドアロケーター
	ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }         // コマンドリスト

	IDXGISwapChain4* GetSwapChain() { return swapChain_.Get(); }                       // スワップチェーン
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc_; }                // スワップチェーンディスク
	ID3D12Resource* GetSwapChainResource(int32_t num) { return swapChainResources_[num].Get(); }

	ID3D12DescriptorHeap* GetRTVDescriptorHeap() { return rtvDescriptorHeap_.Get(); }  // RTVディスクリプタヒープ
	D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() { return rtvDesc_; }                    // RTVディスク
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandle(uint32_t num) { return rtvHandles_[num]; } // RTVハンドル
	ID3D12DescriptorHeap* GetSRVDescriptorHeap() { return srvDescriptorHeap_.Get(); }  // SRVディスクリプタヒープ
	uint32_t GetRTVSize() { return descriptorSizeRTV_; }                               // RTVディスクリプタサイズ
	uint32_t GetSRVSize() { return descriptorSizeSRV_; }                               // SRVディスクリプタサイズ
	uint32_t GetDSVSize() { return descriptorSizeDSV_; }                               // DSVディスクリプタサイズ

private:
	///// プライベート変数 /////
	// 画面サイズ
	int32_t clientWidth_ = 0;
	int32_t clientHeight_ = 0;

	// ウィンドウ
	HWND hwnd_ = nullptr;

	// デバイス関連
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;

	// コマンド関連
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;

	// スワップチェーン関連
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc_{};
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources_[2] = { nullptr };

	// ディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc_{};
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;

	// ディスクリプタサイズ
	uint32_t descriptorSizeSRV_;
	uint32_t descriptorSizeRTV_;
	uint32_t descriptorSizeDSV_;

	// プライベート関数
	void CreateDevice();    // デバイス生成
	void CreateCommand();   //コマンド関連の生成
	void CreateSwapChain(); // スワップチェーンの作成
};

