#include "PSOManager.h"
#include <cassert>
#include <format>

PSOManager::PSOManager() {
    shaderCompiler_.InitializeDxc(); // コンストラクタでDXCを初期化
}

PSOManager::~PSOManager() {}

void PSOManager::InitializeDefaultPSO(
    ID3D12Device* device,
    DXGI_FORMAT rtvFormat,
    DXGI_FORMAT dsvFormat) {

    // RootSignatureBuilder を使用してルートシグネチャを作成
    RootSignatureBuilder rootSigBuilder;
    rootSigBuilder.SetFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
    descriptorRange[0].BaseShaderRegister = 0;
    descriptorRange[0].NumDescriptors = 1;
    descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

    D3D12_ROOT_PARAMETER rootParameters[4] = {};
    rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[0].Descriptor.ShaderRegister = 0;
    rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    rootParameters[1].Descriptor.ShaderRegister = 0;
    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;
    rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    rootParameters[3].Descriptor.ShaderRegister = 1;

    for (int i = 0; i < _countof(rootParameters); ++i) {
        rootSigBuilder.AddRootParameter(rootParameters[i]);
    }

    D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
    staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
    staticSamplers[0].ShaderRegister = 0;
    staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    for (int i = 0; i < _countof(staticSamplers); ++i) {
        rootSigBuilder.AddStaticSampler(staticSamplers[i]);
    }

    defaultRootSignature_ = rootSigBuilder.Build(device);

    // シェーダーをコンパイル
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = shaderCompiler_.CompileShader(L"engine/shader/Object3d.VS.hlsl", L"vs_6_0");
    assert(vertexShaderBlob != nullptr);
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = shaderCompiler_.CompileShader(L"engine/shader/Object3d.PS.hlsl", L"ps_6_0");
    assert(pixelShaderBlob != nullptr);

    // InputLayout
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
    inputElementDescs[0].SemanticName = "POSITION";
    inputElementDescs[0].SemanticIndex = 0;
    inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[1].SemanticName = "TEXCOORD";
    inputElementDescs[1].SemanticIndex = 0;
    inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    inputElementDescs[2].SemanticName = "NORMAL";
    inputElementDescs[2].SemanticIndex = 0;
    inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
    inputLayoutDesc.pInputElementDescs = inputElementDescs;
    inputLayoutDesc.NumElements = _countof(inputElementDescs);

    // BlendState
    D3D12_BLEND_DESC blendDesc{};
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

    // RasterizerState
    D3D12_RASTERIZER_DESC rasterizerDesc{};
    rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
    rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    // DepthStencilState
    D3D12_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    // GraphicsPipelineStateBuilder を使用してPSOを作成
    GraphicsPipelineStateBuilder psoBuilder;
    defaultGraphicsPipelineState_ = psoBuilder
        .SetRootSignature(defaultRootSignature_.Get())
        .SetInputLayout(inputLayoutDesc)
        .SetVertexShader(vertexShaderBlob.Get())
        .SetPixelShader(pixelShaderBlob.Get())
        .SetBlendState(blendDesc)
        .SetRasterizerState(rasterizerDesc)
        .SetDepthStencilState(depthStencilDesc)
        .AddRenderTargetFormat(rtvFormat)
        .SetDepthStencilViewFormat(dsvFormat)
        .SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
        .Build(device);
}

void PSOManager::InitializeLinePSO(
    ID3D12Device* device,
    DXGI_FORMAT rtvFormat,
    DXGI_FORMAT dsvFormat) {

    // RootSignatureBuilder を使用してライン用ルートシグネチャを作成
    RootSignatureBuilder lineRootSigBuilder;
    lineRootSigBuilder.SetFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    D3D12_ROOT_PARAMETER lineRootParameters[1] = {};
    lineRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    lineRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    lineRootParameters[0].Descriptor.ShaderRegister = 0;
    lineRootSigBuilder.AddRootParameter(lineRootParameters[0]);

    lineRootSignature_ = lineRootSigBuilder.Build(device);

    // シェーダーをコンパイル
    Microsoft::WRL::ComPtr<IDxcBlob> lineVertexShaderBlob = shaderCompiler_.CompileShader(L"engine/shader/Grid.VS.hlsl", L"vs_6_0");
    assert(lineVertexShaderBlob != nullptr);
    Microsoft::WRL::ComPtr<IDxcBlob> linePixelShaderBlob = shaderCompiler_.CompileShader(L"engine/shader/Grid.PS.hlsl", L"ps_6_0");
    assert(linePixelShaderBlob != nullptr);

    // InputLayout
    D3D12_INPUT_ELEMENT_DESC lineInputElementDescs[2] = {};
    lineInputElementDescs[0].SemanticName = "POSITION";
    lineInputElementDescs[0].SemanticIndex = 0;
    lineInputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    lineInputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    lineInputElementDescs[1].SemanticName = "TEXCOORD";
    lineInputElementDescs[1].SemanticIndex = 0;
    lineInputElementDescs[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    lineInputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    D3D12_INPUT_LAYOUT_DESC lineInputLayoutDesc{};
    lineInputLayoutDesc.pInputElementDescs = lineInputElementDescs;
    lineInputLayoutDesc.NumElements = _countof(lineInputElementDescs);

    // BlendState
    D3D12_BLEND_DESC lineBlendDesc{};
    lineBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // RasterizerState
    D3D12_RASTERIZER_DESC lineRasterizerDesc{};
    lineRasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
    lineRasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

    // DepthStencilState
    D3D12_DEPTH_STENCIL_DESC lineDepthStencilDesc = {};
    lineDepthStencilDesc.DepthEnable = true;
    lineDepthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    lineDepthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    // GraphicsPipelineStateBuilder を使用してライン用PSOを作成
    GraphicsPipelineStateBuilder linePsoBuilder;
    lineGraphicsPipelineState_ = linePsoBuilder
        .SetRootSignature(lineRootSignature_.Get())
        .SetInputLayout(lineInputLayoutDesc)
        .SetVertexShader(lineVertexShaderBlob.Get())
        .SetPixelShader(linePixelShaderBlob.Get())
        .SetBlendState(lineBlendDesc)
        .SetRasterizerState(lineRasterizerDesc)
        .SetDepthStencilState(lineDepthStencilDesc)
        .AddRenderTargetFormat(rtvFormat)
        .SetDepthStencilViewFormat(dsvFormat)
        .SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)
        .Build(device);
}