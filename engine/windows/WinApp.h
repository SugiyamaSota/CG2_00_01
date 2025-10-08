#pragma once
#include<Windows.h>
#include<stdint.h>

class WinApp {
public:
	/// --- インスタンス関連 ---
	// 生成
	static WinApp* GetInstance();
	// 破棄
	static void DestroyInstance();
	// コピーを禁止
	WinApp(const WinApp&) = delete;
	WinApp& operator=(const WinApp&) = delete;

	/// --- 汎用関数 ---
	// デストラクタ
	~WinApp();
	// 更新
	void Update();


	/// --- 取得関数 ---
	// クライアント領域
	int32_t GetClientWidth() { return kClientWidth; }
	int32_t GetClientHeight() { return kClientHeight; }
	// ウィンドウハンドル
	HWND GetHWND() { return hwnd_; }

private:
	// シングルトンインスタンスを保持
	static WinApp* instance_;

	// プライベートコンストラクタ
	WinApp();

	/// --- 汎用関数 ---
	// 初期化 (GetInstanceから内部的に呼び出されます)
	void Initialize(); // Typoを修正: Initialzie -> Initialize


	/// --- 変数 --- 
	// クライアント領域
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;
	// ウィンドウハンドル
	HWND hwnd_ = nullptr;


	/// --- 関数 ---
	// ウィンドウプロシージャ
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
};