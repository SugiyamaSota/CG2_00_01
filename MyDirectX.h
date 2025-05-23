#pragma once
#include<Windows.h>
#include<cstdint>

//ImGUi関連
#include"externals/imgui/imgui.h"
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class MyDirectX
{
private: // クラス変数

	//クライアント領域のサイズ
	const int32_t kClientWidth_ = 1280;
	const int32_t kClientHeight_ = 720;

	RECT wrc_ = { 0,0,kClientWidth_ ,kClientHeight_ };

private: // クラス関数

	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
	void Initialize();

	void Update();

	void Finalize();
};

