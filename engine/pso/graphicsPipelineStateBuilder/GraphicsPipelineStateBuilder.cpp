#include "GraphicsPipelineStateBuilder.h"
#include <cassert>

GraphicsPipelineStateBuilder::GraphicsPipelineStateBuilder() {
    // デフォルト値を設定
    ZeroMemory(&desc_, sizeof(desc_));
    desc_.SampleDesc.Count = 1; // デフォルトは1
    desc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // デフォルトマスク
}

GraphicsPipelineStateBuilder& GraphicsPipelineStateBuilder::SetRootSignature(ID3D12RootSignature* rootSignature) {
    desc_.pRootSignature = rootSignature;
    return *this;
}

GraphicsPipelineStateBuilder& GraphicsPipelineStateBuilder::SetInputLayout(const D3D12_INPUT_LAYOUT_DESC& inputLayout) {
    desc_.InputLayout = inputLayout;
    return *this;
}

GraphicsPipelineStateBuilder& GraphicsPipelineStateBuilder::SetVertexShader(IDxcBlob* vsBlob) {
    desc_.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
    return *this;
}

GraphicsPipelineStateBuilder& GraphicsPipelineStateBuilder::SetPixelShader(IDxcBlob* psBlob) {
    desc_.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
    return *this;
}

GraphicsPipelineStateBuilder& GraphicsPipelineStateBuilder::SetBlendState(const D3D12_BLEND_DESC& blendState) {
    desc_.BlendState = blendState;
    return *this;
}

GraphicsPipelineStateBuilder& GraphicsPipelineStateBuilder::SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerState) {
    desc_.RasterizerState = rasterizerState;
    return *this;
}

GraphicsPipelineStateBuilder& GraphicsPipelineStateBuilder::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilState) {
    desc_.DepthStencilState = depthStencilState;
    return *this;
}

GraphicsPipelineStateBuilder& GraphicsPipelineStateBuilder::AddRenderTargetFormat(DXGI_FORMAT format) {
    rtvFormats_.push_back(format);
    return *this;
}

GraphicsPipelineStateBuilder& GraphicsPipelineStateBuilder::SetDepthStencilViewFormat(DXGI_FORMAT format) {
    desc_.DSVFormat = format;
    return *this;
}

GraphicsPipelineStateBuilder& GraphicsPipelineStateBuilder::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type) {
    desc_.PrimitiveTopologyType = type;
    return *this;
}

GraphicsPipelineStateBuilder& GraphicsPipelineStateBuilder::SetSampleDesc(const DXGI_SAMPLE_DESC& sampleDesc) {
    desc_.SampleDesc = sampleDesc;
    return *this;
}

GraphicsPipelineStateBuilder& GraphicsPipelineStateBuilder::SetSampleMask(UINT mask) {
    desc_.SampleMask = mask;
    return *this;
}


Microsoft::WRL::ComPtr<ID3D12PipelineState> GraphicsPipelineStateBuilder::Build(ID3D12Device* device) {
    // RTVFormatsをコピー
    desc_.NumRenderTargets = static_cast<UINT>(rtvFormats_.size());
    for (size_t i = 0; i < rtvFormats_.size(); ++i) {
        desc_.RTVFormats[i] = rtvFormats_[i];
    }

    Microsoft::WRL::ComPtr<ID3D12PipelineState> pso = nullptr;
    HRESULT hr = device->CreateGraphicsPipelineState(&desc_, IID_PPV_ARGS(pso.GetAddressOf()));
    assert(SUCCEEDED(hr));
    return pso;
}