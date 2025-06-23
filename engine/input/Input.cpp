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

    hr = directInput_->CreateDevice(GUID_SysKeyboard, keyboard_.GetAddressOf(), NULL);
    assert(SUCCEEDED(hr));

    hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
    assert(SUCCEEDED(hr));

    hr = keyboard_->SetCooperativeLevel(
        hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
    assert(SUCCEEDED(hr));
}

void Input::Update() {
    // 前のフレームのキー状態を保存
    memcpy(prevKeyState_, keyState_, sizeof(keyState_));

    // 現在のキー状態を取得
    keyboard_->Acquire();
    keyboard_->GetDeviceState(sizeof(keyState_), keyState_);
}

bool Input::IsPress(int DIK_KEY) {
    return (keyState_[DIK_KEY] & 0x80);
}

bool Input::IsTrigger(int DIK_KEY) {
    return (keyState_[DIK_KEY] & 0x80) && !(prevKeyState_[DIK_KEY] & 0x80);
}