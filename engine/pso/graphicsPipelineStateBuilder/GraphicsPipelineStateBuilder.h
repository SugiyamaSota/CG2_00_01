#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include<dxcapi.h>

class GraphicsPipelineStateBuilder {
public:
    GraphicsPipelineStateBuilder();

    GraphicsPipelineStateBuilder& SetRootSignature(ID3D12RootSignature* rootSignature);
    GraphicsPipelineStateBuilder& SetInputLayout(const D3D12_INPUT_LAYOUT_DESC& inputLayout);
    GraphicsPipelineStateBuilder& SetVertexShader(IDxcBlob* vsBlob);
    GraphicsPipelineStateBuilder& SetPixelShader(IDxcBlob* psBlob);
    GraphicsPipelineStateBuilder& SetBlendState(const D3D12_BLEND_DESC& blendState);
    GraphicsPipelineStateBuilder& SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerState);
    GraphicsPipelineStateBuilder& SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilState);
    GraphicsPipelineStateBuilder& AddRenderTargetFormat(DXGI_FORMAT format);
    GraphicsPipelineStateBuilder& SetDepthStencilViewFormat(DXGI_FORMAT format);
    GraphicsPipelineStateBuilder& SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
    GraphicsPipelineStateBuilder& SetSampleDesc(const DXGI_SAMPLE_DESC& sampleDesc);
    GraphicsPipelineStateBuilder& SetSampleMask(UINT mask);

    Microsoft::WRL::ComPtr<ID3D12PipelineState> Build(ID3D12Device* device);

private:
    D3D12_GRAPHICS_PIPELINE_STATE_DESC desc_;
    std::vector<DXGI_FORMAT> rtvFormats_;
};