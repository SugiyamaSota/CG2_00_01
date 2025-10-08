#include"WinApp.h"
#include"../imgui/ImGuiManager.h"

WinApp* WinApp::instance_ = nullptr;

WinApp::WinApp() {
}

WinApp* WinApp::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = new WinApp();
		instance_->Initialize();
	}
	return instance_;
}

void WinApp::DestroyInstance() {
	delete instance_;
	instance_ = nullptr;
}

void WinApp::Initialize() {
	//
	HRESULT hr = CoInitializeEx(0, COINIT_MULTITHREADED);

	// ウィンドウクラスの生成と登録
	WNDCLASS wc{};
	wc.lpfnWndProc = WindowProc;
	wc.lpszClassName = L"CG2WindowClass";
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = (LoadCursor(nullptr, IDC_ARROW));
	RegisterClass(&wc);

	//
	RECT wrc = { 0,0,kClientWidth,kClientHeight };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウの生成と表示
	hwnd_ = CreateWindow(
		wc.lpszClassName,
		L"CG2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr);
	ShowWindow(hwnd_, SW_SHOW);
}

WinApp::~WinApp() {
	CoUninitialize();
	CloseWindow(hwnd_);
}

bool WinApp::ProcessMessage() {
	MSG msg{};

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT)
	{
		return true;
	}

	return false;
}


LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	// メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
	case WM_KEYUP: // キーが離されたとき
		// Escキーが押されたら
		if (wparam == VK_ESCAPE) {
			// Windowを閉じるメッセージを送信する
			PostMessage(hwnd, WM_CLOSE, 0, 0);
		}
		return 0;
		// ウィンドウが破棄された
	case WM_DESTROY:
		// OSに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
