#pragma once
#include"../audio/AudioPlayer.h"
#include"../camera/DebugCamera.h"
#include"../common/DirectXCommon.h"
#include"../function/Utility.h"
#include"../imgui/ImGuiManager.h"
#include"../input/InputKey.h"
#include"../math/Struct.h"
#include"../math/Vector.h"
#include"../math/Matrix.h"
#include"../math/Convert.h"
#include"../math/Collision.h"
#include"../model/Model.h"
#include"../pso/PSO.h"
#include"../texture/TextureManager.h"

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

	InputKey* GetKey() { return key_; }

private:
	D3DResourceLeakChecker leakChecker_;
	InputKey* key_ = nullptr;

};

