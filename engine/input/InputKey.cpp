#include "InputKey.h"
#include <cassert>

#pragma comment(lib,"dinput8.lib")

// ★ 静的メンバの初期化
InputKey* InputKey::sInstance_ = nullptr;

// ★ シングルトンインスタンスを取得するメソッドの実装
InputKey* InputKey::GetInstance() {
    if (sInstance_ == nullptr) {
        sInstance_ = new InputKey();
    }
    return sInstance_;
}

// コンストラクタはプライベートだが、実装は必要
InputKey::InputKey() {
    // コンストラクタでの初期化はここでは行わず、Initialize() で行うのが適切です。
    // Initialize() にDirectXInputの初期化処理が集約されています。
}

void InputKey::Initialize(HINSTANCE hInstance, HWND hwnd) {
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

void InputKey::Update() {
    // 前のフレームのキー状態を保存
    memcpy(prevKeyState_, keyState_, sizeof(keyState_));

    // 現在のキー状態を取得
    keyboard_->Acquire();
    keyboard_->GetDeviceState(sizeof(keyState_), keyState_);
}

bool InputKey::IsPress(int DIK_KEY) {
    return (keyState_[DIK_KEY] & 0x80);
}

bool InputKey::IsTrigger(int DIK_KEY) {
    return (keyState_[DIK_KEY] & 0x80) && !(prevKeyState_[DIK_KEY] & 0x80);
}