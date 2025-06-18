#include "ImGuiManager.h"

ImGuiManager::ImGuiManager(HWND hwnd, ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ID3D12DescriptorHeap* srvDescriptorHeap) {
	hwnd_ = hwnd;
	device_ = device;
	commandList_ = commandList;
	srvDescriptorHeap_ = srvDescriptorHeap;
}

ImGuiManager::~ImGuiManager() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiManager::Initialize() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hwnd_);
	// ImGui_ImplDX12_Init の引数を修正
	// SRVディスクリプタヒープを渡す
	ImGui_ImplDX12_Init(
		device_.Get(),
		2, // フレームバッファの数 (スワップチェーンのバッファ数と同じ)
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, // レンダーターゲットのフォーマット
		srvDescriptorHeap_.Get(), // ImGuiが使用するSRVディスクリプタヒープ
		srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart(), // ImGuiのフォントテクスチャのCPUハンドル
		srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart()
	);// ImGuiのフォントテクスチャのGPUハンドル
}

void ImGuiManager::NewFrame() {
	//フレームの最初
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX12_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::EndFrame() {
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList_.Get());
}