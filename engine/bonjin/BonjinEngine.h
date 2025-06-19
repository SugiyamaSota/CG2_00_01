#pragma once
#include"../common/DirectXCommon.h"
#include"../input/InputKey.h"

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {
		Microsoft::WRL::ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf())))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};


class BonjinEngine {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(HINSTANCE hInstance, int32_t kClientWidth, int32_t ClientHeight);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~BonjinEngine();

	/// <summary>
	/// 最初のフレームに呼び出し
	/// </summary>
	void NewFrame();

	/// <summary>
	/// 最後のフレームに呼び出し
	/// </summary>
	void EndFrame();

	DirectXCommon* GetCommon() { return common_; }
	InputKey* GetKey() { return key_; }

private:
	//リソースリークチェック
	D3DResourceLeakChecker leakChecker_;

	DirectXCommon* common_ = nullptr;
	InputKey* key_ = nullptr;
};

