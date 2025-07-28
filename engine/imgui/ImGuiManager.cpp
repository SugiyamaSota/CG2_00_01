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

void ImGuiManager::DrawModelPropertiesUI(Model& model, const std::string& label) { // ★関数名と引数変更★
    // Model クラスから必要なデータを取得
    WorldTransform& transform = model.GetTransform();
    Material& materialData = model.GetMaterialData();

    // ImGui::SetNextWindowPos(...) や ImGui::Begin/End はここで削除します

    // CollapsingHeaderのラベルにモデル名を付与する
    if (ImGui::CollapsingHeader((label + " : Transform").c_str())) {
        // スケール
        std::string scaleLabel = label + ": scale";
        ImGui::DragFloat3(scaleLabel.c_str(), &transform.scale.x, 1.0f, -100.0f, 100.0f);

        // 回転
        std::string rotateLabel = label + ": rotate";
        ImGui::DragFloat3(rotateLabel.c_str(), &transform.rotate.x, 1.0f, -100.0f, 100.0f);

        // 移動
        std::string translateLabel = label + ": translate";
        ImGui::DragFloat3(translateLabel.c_str(), &transform.translate.x, 1.0f, -100.0f, 100.0f);
    }
    if (ImGui::CollapsingHeader((label + " : Material").c_str())) { // ここもラベルを付与すると分かりやすい
        // 色
        std::string colorLabel = label + ": color";
        ImGui::ColorEdit4(colorLabel.c_str(), &materialData.color.x);

        // ライティング
        std::string enableLightingLabel = label + " : Enable Lighting";
        bool isLightingEnabled = (materialData.enableLighting != 0);

        if (ImGui::Checkbox(enableLightingLabel.c_str(), &isLightingEnabled)) {
            materialData.enableLighting = isLightingEnabled ? 1 : 0;
        }
    }
    // ImGui::End() も削除します
}

void ImGuiManager::RegisterModel(Model* model, const std::string& name) {
    registeredModels_.push_back(model);
    registeredModelNames_.push_back(name);
}


// ★DrawCombinedModelDebugUI の実装を変更★
// 変更前: void ImGuiManager::DrawCombinedModelDebugUI(const std::vector<Model*>& models, const std::vector<std::string>& modelNames, int& selectedIndex) {
// 変更後:
void ImGuiManager::DrawCombinedModelDebugUI(int& selectedIndex) { // ★引数変更★
    // ウィンドウの初期位置とサイズを設定 (任意)
    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(350.0f, 600.0f), ImGuiCond_Appearing);

    ImGui::Begin("Combined Model Debug UI");

    // 内部のregisteredModels_を使用
    if (registeredModels_.empty()) { // ★変更: models -> registeredModels_★
        ImGui::Text("No models registered for debug.");
        ImGui::End();
        return;
    }

    // モデル選択用のコンボボックス
    std::vector<const char*> modelNamesCStr;
    // 内部のregisteredModelNames_を使用
    for (const auto& name : registeredModelNames_) { // ★変更: modelNames -> registeredModelNames_★
        modelNamesCStr.push_back(name.c_str());
    }

    ImGui::Text("Select Model:");
    ImGui::Combo("##modelSelector", &selectedIndex, modelNamesCStr.data(), static_cast<int>(modelNamesCStr.size()));

    ImGui::Separator();

    // 選択されたモデルのプロパティを描画
    // 内部のregisteredModels_を使用
    if (selectedIndex >= 0 && selectedIndex < registeredModels_.size()) { // ★変更: models.size() -> registeredModels_.size()★
        Model* currentModel = registeredModels_[selectedIndex]; // ★変更: models -> registeredModels_★
        std::string currentModelLabel = registeredModelNames_[selectedIndex]; // ★変更: modelNames -> registeredModelNames_★
        DrawModelPropertiesUI(*currentModel, currentModelLabel);
    } else {
        ImGui::Text("Invalid model selected or no models registered.");
    }

    ImGui::End();
}