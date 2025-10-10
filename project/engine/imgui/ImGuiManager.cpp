#include "ImGuiManager.h"

#include"../windows/WinApp.h"
#include"../common/DirectXCommon.h"

// 静的メンバ変数の初期化
ImGuiManager* ImGuiManager::sInstance = nullptr; // 静的メンバ変数を初期化

ImGuiManager* ImGuiManager::GetInstance() {
	if (sInstance == nullptr) { // インスタンスがまだ作成されていない場合
		sInstance = new ImGuiManager(); // 新しいインスタンスを作成
	}
	return sInstance; // 既存のインスタンスを返す
}

ImGuiManager::ImGuiManager() {
}

void ImGuiManager::DestroyInstance() {
	delete sInstance; // ここでデストラクタが呼ばれる
	sInstance = nullptr;
}


ImGuiManager::~ImGuiManager() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	// シングルトンインスタンスをnullptrに戻す
}

void ImGuiManager::Initialize() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(WinApp::GetInstance()->GetHWND());
	ImGui_ImplDX12_Init(
		DirectXCommon::GetInstance()->GetDevice(),
		2,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		DirectXCommon::GetInstance()->GetSRVDescriptorHeap(),
		DirectXCommon::GetInstance()->GetSRVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		DirectXCommon::GetInstance()->GetSRVDescriptorHeap()->GetGPUDescriptorHandleForHeapStart()
	);
}

void ImGuiManager::NewFrame() {
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX12_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::EndFrame() {
	ImGui::Render();
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DirectXCommon::GetInstance()->GetCommandList());
}