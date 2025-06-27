#include "Input.h"
#include <cassert>
#include <memory> // std::unique_ptr を使用するために追加

#pragma comment(lib,"dinput8.lib")

// シングルトンインスタンスを保持する静的ポインタ
// スレッドセーフなシングルトンのために std::call_once や std::mutex を使用することも検討できますが、
// ここではシンプルな実装としています。
static std::unique_ptr<Input> sInputInstance;

Input* Input::GetInstance() {
	if (!sInputInstance) {
		sInputInstance = std::unique_ptr<Input>(new Input());
	}
	return sInputInstance.get();
}

void Input::Initialize(HINSTANCE hInstance, HWND hwnd) {
	HRESULT hr = DirectInput8Create(
		hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)directInput_.GetAddressOf(), nullptr);
	assert(SUCCEEDED(hr));

	// キーボードデバイスの取得
	hr = directInput_->CreateDevice(GUID_SysKeyboard, keyboard_.GetAddressOf(), NULL);
	assert(SUCCEEDED(hr));

	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));

	hr = keyboard_->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));

	// マウスデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouse_, NULL);
	assert(SUCCEEDED(hr));

	hr = mouse_->SetDataFormat(&c_dfDIMouse2);
	assert(SUCCEEDED(hr));

	hr = mouse_->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	assert(SUCCEEDED(hr));

	mouse_->Acquire();
}


void Input::Update() {
    // 前のフレームのキー状態を保存
    memcpy(prevKeyState_, keyState_, sizeof(keyState_));

    // 現在のキー状態を取得
    // キーボードも同様にエラーハンドリングを強化することを推奨します
    keyboard_->Acquire(); // キーボードもAcquire()が必要
    HRESULT hrKey = keyboard_->GetDeviceState(sizeof(keyState_), keyState_);
    if (FAILED(hrKey)) {
        if ((hrKey == DIERR_INPUTLOST) || (hrKey == DIERR_NOTACQUIRED)) {
            // キーボードの再取得も試みる
            keyboard_->Acquire();
        }
        ZeroMemory(&keyState_, sizeof(keyState_)); // エラー時は状態をクリア
    }

    // マウスの状態を更新
    prevMouseState_ = mouseState_; // 前のフレームの状態を保存

    HRESULT hrMouse = mouse_->GetDeviceState(sizeof(mouseState_), &mouseState_);
    if (FAILED(hrMouse)) {
        // デバイスがロストした場合などに対応
        if ((hrMouse == DIERR_INPUTLOST) || (hrMouse == DIERR_NOTACQUIRED)) {
            // ここでAcquireを呼び出すのが正しいパターン
            HRESULT hrAcquire = mouse_->Acquire();
            // Acquireが成功したかどうかも確認する（S_OK または S_FALSE は成功とみなす）
            if (FAILED(hrAcquire)) {
                // Acquire自体が失敗した場合、エラーログを出力するなど
                char buffer[64];
                sprintf_s(buffer, sizeof(buffer), "Failed to re-acquire mouse: HRESULT = 0x%X\n", hrAcquire);
                OutputDebugStringA(buffer);
            }
        }
        // それ以外のエラー、または再取得後もまだ状態が取得できない場合は状態をクリア
        ZeroMemory(&mouseState_, sizeof(mouseState_));
        // デバッグ用にエラーコードを出力
        char buffer[64];
        sprintf_s(buffer, sizeof(buffer), "Mouse GetDeviceState failed: HRESULT = 0x%X\n", hrMouse);
        OutputDebugStringA(buffer);
        // ここでreturnしてもよいし、このまま継続して次のフレームで再試行でもよい
        // 今回はとりあえず状態をクリアして続行
    }
}

bool Input::IsPress(int DIK_KEY) {
	return (keyState_[DIK_KEY] & 0x80);
}

bool Input::IsTrigger(int DIK_KEY) {
	return (keyState_[DIK_KEY] & 0x80) && !(prevKeyState_[DIK_KEY] & 0x80);
}

bool Input::IsMousePress(int button) {
	return (mouseState_.rgbButtons[button] & 0x80) != 0;
}

bool Input::IsMouseTrigger(int button) {
	return (mouseState_.rgbButtons[button] & 0x80) != 0 &&
		(prevMouseState_.rgbButtons[button] & 0x80) == 0;
}

long Input::GetMouseDeltaX() {
	return mouseState_.lX;
}

long Input::GetMouseDeltaY() {
	return mouseState_.lY;
}

long Input::GetMouseWheel() {
	return mouseState_.lZ;
}