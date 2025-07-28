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

#include<string>
#include<vector>

// ImGui関連の外部ライブラリヘッダー
#include "../../externals/imgui/imgui.h"
#include "../../externals/imgui/imgui_impl_dx12.h"
#include "../../externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Model;

class ImGuiManager
{
public:
	// シングルトンインスタンスの取得
	static ImGuiManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 最初のフレーム
	/// </summary>
	void NewFrame();

	/// <summary>
	/// 最後のフレーム
	/// </summary>
	void EndFrame();

	static void DestroyInstance();

	/// <summary>
	/// モデルのデバッグGUI (個々のモデルのプロパティを描画するヘルパー関数として再利用)
	/// </summary>
	void DrawModelPropertiesUI(Model& model, const std::string& label);

	/// <summary>
	/// モデルをImGuiのデバッグ対象として登録する
	/// </summary>
	void RegisterModel(Model* model, const std::string& name);

	/// <summary>
	/// 全てのモデルのデバッグGUIを統合して描画
	/// </summary>
	void DrawCombinedModelDebugUI(int& selectedIndex);

private:
	// コンストラクタとデストラクタをprivateにする
	ImGuiManager();
	~ImGuiManager();

	// コピーコンストラクタと代入演算子を禁止する
	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager& operator=(const ImGuiManager&) = delete;

	// 静的メンバ変数の追加
	static ImGuiManager* sInstance;

	// 登録されたモデルとその名前を登録するリスト
	std::vector<Model*> registeredModels_;
	std::vector<std::string> registeredModelNames_;
};