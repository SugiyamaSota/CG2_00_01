#pragma once
#include<Windows.h>

// DirectX関連のSDKヘッダー
#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <dxcapi.h>
#include <dbghelp.h>
#include <strsafe.h>
#include <wrl/client.h>

// ImGui関連の外部ライブラリヘッダー
#include "../../externals/imgui/imgui.h"
#include "../../externals/imgui/imgui_impl_dx12.h"
#include "../../externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class ImGuiManager
{
public:
	ImGuiManager(HWND hwnd, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srvDescriptorHeap);
	~ImGuiManager();

	void Initialize();
	void NewFrame();
	void EndFrame();

private:
	HWND hwnd_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap_ = nullptr;
};

