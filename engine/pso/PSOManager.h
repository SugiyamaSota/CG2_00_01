#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <string>
#include <vector>
#include <dxcapi.h>

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
	ID3D12PipelineState* GetDefaultPipelineState() const { return defaultGraphicsPipelineState_.Get(); }

	// グリッド描画用のオブジェクトのゲッター
	ID3D12RootSignature* GetLineRootSignature() const { return lineRootSignature_.Get(); }
	ID3D12PipelineState* GetLinePipelineState() const { return lineGraphicsPipelineState_.Get(); }

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> defaultRootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> defaultGraphicsPipelineState_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> lineRootSignature_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> lineGraphicsPipelineState_;

	// シェーダーコンパイラー
	ShaderCompiler shaderCompiler_;
};