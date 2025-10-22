#include "BonjinEngine.h"
#include<chrono>
#include<fstream>

// DirectX関連のライブラリリンク
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")
#pragma comment(lib,"Dbghelp.lib")

// 現在時刻を取得
std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
// ログファイルの名前にコンマ何秒はいらないので、削って秒にする
std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
// 日本時間に変換
std::chrono::zoned_time localTime{ std::chrono::current_zone(),nowSeconds };
// formatを使って年月日_時分秒の文字列に変換
std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
// 時刻を使ってファイル名決定
std::string logFilePath = std::string("logs/") + dateString + ".log";
// ファイルを使って書き込み準備
std::ofstream logStream(logFilePath);

void BonjinEngine::Initialize() {
	// directXcommon、Input、テクスチャのインスタンスを取得
	DirectXCommon::GetInstance();
	WinApp::GetInstance();
	Input::GetInstance()->Initialize(WinApp::GetInstance()->GetHInstance(), WinApp::GetInstance()->GetHWND());
	Input::GetInstance()->SetMouseLock(true);
	TextureManager::GetInstance();
	ImGuiManager::GetInstance()->Initialize();
	Time::GetInstance();
}

void BonjinEngine::Finalize() {
	// directXcommon、Input、テクスチャのインスタンスを破壊
	ImGuiManager::DestroyInstance();
	TextureManager::DestroyInstance();
	WinApp::GetInstance()->DestroyInstance();
	DirectXCommon::GetInstance()->DestroyInstance();
	Time::DestroyInstance();
}

void BonjinEngine::NewFrame() {
	Time::GetInstance()->Update();
	Input::GetInstance()->Update();
	ID3D12DescriptorHeap* descriptorHeaps[] = { DirectXCommon::GetInstance()->GetSRVDescriptorHeap() };
	DirectXCommon::GetInstance()->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	ImGuiManager::GetInstance()->NewFrame();
}

void BonjinEngine::PreDraw() {
	DirectXCommon::GetInstance()->NewFeame();
}

void BonjinEngine::EndFrame() {
	ImGuiManager::GetInstance()->EndFrame();
	DirectXCommon::GetInstance()->EndFrame();
}
