#pragma once
#include<Windows.h>
#include<stdint.h>
#include<filesystem>

class WinApp {
public:
	/// --- インスタンス関連 ---
	/// <summary>
	/// インスタンスを生成
	/// </summary>
	static WinApp* GetInstance();

	/// <summary>
	/// インスタンスを破棄
	/// </summary>
	static void DestroyInstance();

	/// <summary>
	/// コピーコンストラクタを削除
	/// </summary>
	/// <param name=""></param>
	WinApp(const WinApp&) = delete;

	/// <summary>
	/// コピー演算子を削除
	/// </summary>
	WinApp& operator=(const WinApp&) = delete;

	/// --- 汎用関数 ---
	/// <summary>
	/// デストラクタ
	/// </summary>
	~WinApp();

	/// <summary>
	/// ウィンドウ管理
	/// </summary>
	/// <returns>ウィンドウの有無</returns>
	bool ProcessMessage();


	/// --- 取得関数 ---
	/// <summary>
	/// クライアント領域の幅を取得
	/// </summary>
	/// <returns>画面の幅</returns>
	int32_t GetClientWidth() { return kClientWidth; }

	/// <summary>
	/// クライアント領域の高さを取得
	/// </summary>
	/// <returns>画面の高さ</returns>
	int32_t GetClientHeight() { return kClientHeight; }

	/// <summary>
	/// ウィンドウハンドルを取得
	/// </summary>
	/// <returns>ウィンドウハンドル</returns>
	HWND GetHWND() { return hwnd_; }

	/// <summary>
	/// インスタンスハンドルを取得
	/// </summary>
	/// <returns>インスタンスハンドル</returns>
	HINSTANCE GetHInstance() { return hInstance_; }

private:
	/// --- 変数 --- 
	// シングルトンインスタンス
	static WinApp* instance_;

	// クライアント領域
	const int32_t kClientWidth = 1280;
	const int32_t kClientHeight = 720;

	// ウィンドウハンドル
	HWND hwnd_ = nullptr;

	// インスタンスハンドル
	HINSTANCE hInstance_ = nullptr;

	/// --- 関数 ---
	/// <summary>
	/// 空のコンストラクタ
	/// </summary>
	WinApp();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();

	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	/// <summary>
	/// ダンプの設定
	/// </summary>
	static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception);
};