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

enum class PrimitiveType {
	kModel,   // モデル
	kGrid,    // グリッド
	kCount,
};

enum class ShaderStage {
	kVertex,
	kPixel,
	kCount,
};

class PSOManager {
public:
	PSOManager();
	~PSOManager();

	void Initialize(
		ID3D12Device* device,
		DXGI_FORMAT rtvFormat,
		DXGI_FORMAT dsvFormat);

	// RootSignature
	ID3D12RootSignature* GetRootSignature(PrimitiveType type) const {
		return rootSignatures_[static_cast<size_t>(type)].Get();
	}

	// PipelineState
	ID3D12PipelineState* GetPipelineState(PrimitiveType type, BlendMode mode) const {
		return graphicsPipelineStates_[static_cast<size_t>(type)][static_cast<size_t>(mode)].Get();
	}

private:
	/// --- 変数 ---
	// RootSignatureを形状ごとに管理
	std::array<
		Microsoft::WRL::ComPtr<ID3D12RootSignature>,
		static_cast<size_t>(PrimitiveType::kCount)
	> rootSignatures_;

	// pipelineStateを形状とブレンドごとに管理
	std::array<
		std::array<
		Microsoft::WRL::ComPtr<ID3D12PipelineState>,
		static_cast<size_t>(BlendMode::kCount)
		>,
		static_cast<size_t>(PrimitiveType::kCount)
	> graphicsPipelineStates_;

	// シェーダーコンパイラー
	ShaderCompiler shaderCompiler_;

	// ⭐ モデルの入力要素ディスクリプタを格納するメンバ配列
	static const size_t kModelInputElements = 3;
	std::array<
		D3D12_INPUT_ELEMENT_DESC,
		kModelInputElements
	> modelInputElementDescs_;

	// ⭐ グリッドの入力要素ディスクリプタを格納するメンバ配列
	static const size_t kGridInputElements = 2;
	std::array<
		D3D12_INPUT_ELEMENT_DESC,
		kGridInputElements
	> gridInputElementDescs_;

	// 形状ごとのインプットレイアウトディスク
	std::array<
		D3D12_INPUT_LAYOUT_DESC, 
		static_cast<size_t>(PrimitiveType::kCount)>
		inputLayoutDescs_;

	// ラスタライザーディスク
	D3D12_RASTERIZER_DESC rasterizerDesc_{};

	// シェーダー
	std::array<
		std::array<
		Microsoft::WRL::ComPtr<IDxcBlob>,
		static_cast<size_t>(ShaderStage::kCount)
		>,
		static_cast<size_t>(PrimitiveType::kCount)
	> shaderBlobs_;

	// 形状ごとのDepthStencil
	std::array<
		D3D12_DEPTH_STENCIL_DESC,
		static_cast<size_t>(PrimitiveType::kCount)
	> depthStencilDescs_;

	/// --- 関数 ---
	void CreateRootSignature(ID3D12Device* device);
	void CompileAllShaders();
	void CreateInputLayout();
	void CreateDepthStencil();
	void CreatePSO(ID3D12Device* device, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat);
};