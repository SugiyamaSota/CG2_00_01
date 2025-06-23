#pragma once

#include <Windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl/client.h>

// Inputクラス
class Input {
private:
    Input() = default;
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    // キー入力
    Microsoft::WRL::ComPtr<IDirectInput8> directInput_;
    Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_;
    BYTE keyState_[256];
    BYTE prevKeyState_[256];

    // マウス入力
    Microsoft::WRL::ComPtr<IDirectInputDevice8> mouse_; // マウスデバイス
    DIMOUSESTATE2 mouseState_; // 現在のマウス状態
    DIMOUSESTATE2 prevMouseState_; // 1フレーム前のマウス状態

public:
    // シングルトンインスタンスを取得する静的メソッド
    static Input* GetInstance();

    // 初期化
    void Initialize(HINSTANCE hInstance, HWND hwnd);

    // 更新
    void Update();

    // キーが押されているか
    bool IsPress(int DIK_KEY);

    // キーがトリガー（押された瞬間）か
    bool IsTrigger(int DIK_KEY);

    // マウスボタンが押されているか
    bool IsMousePress(int button); // button は DIMOFS_BUTTON0 など

    // マウスボタンがトリガー（押された瞬間）か
    bool IsMouseTrigger(int button);

    // マウスのX移動量を取得
    long GetMouseX();

    // マウスのY移動量を取得
    long GetMouseY();

    // マウスのホイール移動量を取得
    long GetMouseWheel();
};