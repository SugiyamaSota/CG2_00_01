#pragma once
#include<stdint.h>

// DirectX関連のSDKヘッダー
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include<vector>

#include"../imgui/ImGuiManager.h"
#include"../pso/PSO.h"
#include"../math/Struct.h"

class DirectXCommon {
public:
	// Singleton インスタンスへのアクセサー
	// 初回呼び出し時にインスタンスを生成し、以降は既存のインスタンスを返します。
	static DirectXCommon* GetInstance(HINSTANCE hInstance = nullptr, int32_t kClientWidth = 0, int32_t ClientHeight = 0);

	static void DestroyInstance();
	// コピーコンストラクタと代入演算子を削除し、コピーを禁止します
	DirectXCommon(const DirectXCommon&) = delete;
	DirectXCommon& operator=(const DirectXCommon&) = delete;

	// デストラクタ
	~DirectXCommon();

	// 初期化処理（コンストラクタで全て行う場合は不要になる可能性があります）
	void Initialize();

	void NewFeame();
	void EndFrame();

	// コマンドリストの実行と完了待機、リセットを行う関数を追加
	void WaitAndResetCommandList();

	//// ゲッター /////
	HWND GetHWND() { return hwnd_; }

	// デバイス関連
	ID3D12Device* GetDevice() { return device_.Get(); }
	IDXGIFactory7* GetDxgiFactory() { return dxgiFactory_.Get(); }

	// コマンド関連
	ID3D12CommandQueue* GetCommandQueue() { return commandQueue_.Get(); }
	ID3D12CommandAllocator* GetCommandAllocator() { return commandAllocator_.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return commandList_.Get(); }

	// スワップチェーン関連
	IDXGISwapChain4* GetSwapChain() { return swapChain_.Get(); }
	DXGI_SWAP_CHAIN_DESC1 GetSwapChainDesc() { return swapChainDesc_; }
	ID3D12Resource* GetSwapChainResource(int32_t index) { return swapChainResources_[index].Get(); }

	// ディスクリプタヒープ
	ID3D12DescriptorHeap* GetRTVDescriptorHeap() { return rtvDescriptorHeap_.Get(); }
	D3D12_RENDER_TARGET_VIEW_DESC GetRTVDesc() { return rtvDesc_; }
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandle(uint32_t index) { return rtvHandles_[index]; }
	ID3D12DescriptorHeap* GetSRVDescriptorHeap() { return srvDescriptorHeap_.Get(); }

	// ディスクリプタサイズ
	uint32_t GetRTVSize() { return descriptorSizeRTV_; }
	uint32_t GetSRVSize() { return descriptorSizeSRV_; }
	uint32_t GetDSVSize() { return descriptorSizeDSV_; }

	// フェンス関連
	ID3D12Fence* GetFence() { return fence_.Get(); }
	uint64_t GetFenceValue() { return fenceValue_; }
	void IncrementFencevalue() { fenceValue_++; }
	HANDLE GetFenceEvent() { return fenceEvent_; }


	// 深度
	ID3D12DescriptorHeap* GetDSVDescriptorHeap() { return dsvDescriptorHeap_.Get(); }

	// 光
	ID3D12Resource* GetDirectionalLightResource() { return directionalLightResource_.Get(); }

	// PSO
	PSO pso;

	//imgui
	ImGuiManager* imguiManager_;

private:
	// プライベートコンストラクタ
	// シングルトンパターンでは外部からのインスタンス化を禁止するため、コンストラクタをprivateにします。
	DirectXCommon(HINSTANCE hInstance, int32_t kClientWidth, int32_t ClientHeight);

	// シングルトンインスタンスを保持する静的メンバー変数
	static DirectXCommon* instance_;

	///// プライベート変数 /////
	// 画面サイズ
	int32_t clientWidth_ = 1280;
	int32_t clientHeight_ = 720;

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

	// フェンス関連
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
	uint64_t fenceValue_ = 0;
	HANDLE fenceEvent_;

	// 深度
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_ = nullptr;
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc_ = {};
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc_ = {};

	// 光
	struct Vector4 {
		float x, y, z, w;
	};

	struct Vector3 {
		float x, y, z;
	};

	struct DirectionalLight {
		Vector4 color;
		Vector3 direction;
		float intentity;
	};
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_ = nullptr;
	DirectionalLight* directionalLightData_ = nullptr;

	/////　行列の宣言 /////
	// ビューポート
	D3D12_VIEWPORT viewport_{};

	// シザー矩形
	D3D12_RECT scissorRect_{};

	// 描画前
	D3D12_RESOURCE_BARRIER barrier_{};

	// プライベート関数
	void CreateDevice();    // デバイス生成
	void CreateCommand();   //コマンド関連の生成
	void CreateSwapChain(); // スワップチェーンの作成
	void CreateFence();
	void CreateDepth();
	void CreateLight();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height);
};