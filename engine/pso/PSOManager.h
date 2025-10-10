#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <string>
#include <vector>
#include <dxcapi.h>
#include<array>

#include "shaderCompiler/ShaderCompiler.h"
#include "rootSignatureBuilder/RootSignatureBuilder.h"
#include "graphicsPipelineStateBuilder/GraphicsPipelineStateBuilder.h"

class PSOManager {
public:
	PSOManager();
	~PSOManager();

	// デフォルトのPSOを初期化
	void InitializeDefaultPSO(
		ID3D12Device* device,
		DXGI_FORMAT rtvFormat,
		DXGI_FORMAT dsvFormat);

	// グリッド描画用のPSOを初期化
	void InitializeLinePSO(
		ID3D12Device* device,
		DXGI_FORMAT rtvFormat,
		DXGI_FORMAT dsvFormat);

	// デフォルトのオブジェクトのゲッター
	ID3D12RootSignature* GetDefaultRootSignature() const { return defaultRootSignature_.Get(); }
	ID3D12PipelineState* GetDefaultPipelineState(BlendMode mode) const {
		return defaultGPS_[static_cast<size_t>(mode)].Get();
	}

	// グリッド描画用のオブジェクトのゲッター
	ID3D12RootSignature* GetLineRootSignature() const { return lineRootSignature_.Get(); }
	ID3D12PipelineState* GetLinePipelineState() const { return lineGraphicsPipelineState_.Get(); }

	// ブレンドモード設定


private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> defaultRootSignature_;
	std::array<Microsoft::WRL::ComPtr<ID3D12PipelineState>, static_cast<size_t>(BlendMode::kCount)>defaultGPS_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> lineRootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> lineGraphicsPipelineState_;

	// シェーダーコンパイラー
	ShaderCompiler shaderCompiler_;
};