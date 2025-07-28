#include "ImGuiManager.h"
#include"../common/DirectXCommon.h"

#include"../bonjin/BonjinEngine.h"

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
	ImGui_ImplWin32_Init(DirectXCommon::GetInstance()->GetHWND());
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

void ImGuiManager::DebugGUIForModel(Model& model) {
	// Model クラスから必要なデータを取得
	WorldTransform& transform = model.GetTransform();
	Material& materialData = model.GetMaterialData();

	ImGui::Begin("debug model");
	if (ImGui::CollapsingHeader("wvpData")) {
		// スケール
		std::string scaleLabel = ": scale";
		ImGui::DragFloat3(scaleLabel.c_str(), &transform.scale.x, 1.0f, -100.0f, 100.0f);

		// 回転
		std::string rotateLabel = ": rotate";
		ImGui::DragFloat3(rotateLabel.c_str(), &transform.rotate.x, 1.0f, -100.0f, 100.0f);

		// 移動
		std::string translateLabel = ": translate";
		ImGui::DragFloat3(translateLabel.c_str(), &transform.translate.x, 1.0f, -100.0f, 100.0f);
	}
	if (ImGui::CollapsingHeader("materialData")) {
		// 色
		std::string colorLabel = ": color";
		ImGui::ColorEdit4(colorLabel.c_str(), &materialData.color.x);

		// ライティング
		std::string enableLightingLabel = " : Enable Lighting";

		bool isLightingEnabled = (materialData.enableLighting != 0);

		if (ImGui::Checkbox(enableLightingLabel.c_str(), &isLightingEnabled)) {
			materialData.enableLighting = isLightingEnabled ? 1 : 0;
		}
	}
	ImGui::End();
}