#pragma once

#include <Windows.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <wrl/client.h>

// Inputクラス
class Input {
private:
    // コンストラクタをprivateにして外部からのインスタンス生成を防ぐ
    Input() = default;
    // コピーコンストラクタと代入演算子を削除してコピーを防ぐ
    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    Microsoft::WRL::ComPtr<IDirectInput8> directInput_;
    Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard_;
    BYTE keyState_[256];
    BYTE prevKeyState_[256];

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
};