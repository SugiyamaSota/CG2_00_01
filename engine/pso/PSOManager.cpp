#include "PSOManager.h"
#include <cassert>
#include <format>

// ⭐ シェーダーファイルパスの定数定義 [PrimitiveType][ShaderStage]
static const wchar_t* kShaderPaths[static_cast<size_t>(PrimitiveType::kCount)]
[static_cast<size_t>(ShaderStage::kCount)] = {
    // kModel
    { L"engine/shader/Object3d.VS.hlsl", L"engine/shader/Object3d.PS.hlsl" },
    // kLine
    { L"engine/shader/Grid.VS.hlsl", L"engine/shader/Grid.PS.hlsl" }
};

// ⭐ シェーダープロファイルの定数定義 [ShaderStage]
static const wchar_t* kShaderProfiles[static_cast<size_t>(ShaderStage::kCount)] = {
    L"vs_6_0", // kVertex
    L"ps_6_0"  // kPixel
};

PSOManager::PSOManager() {
    shaderCompiler_.InitializeDxc(); // コンストラクタでDXCを初期化
}

PSOManager::~PSOManager() {}

void PSOManager::Initialize(ID3D12Device* device,DXGI_FORMAT rtvFormat,DXGI_FORMAT dsvFormat)
{
    /// --- 形状ごとのRoorSignature ---
    CreateRootSignature(device);

    /// --- 共通の初期化 ---
    // RasterizerState
    rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
    rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

    /// --- シェーダー ---
    CompileAllShaders();

    /// --- InputLayout ---
    CreateInputLayout();

    /// --- DepthStencilState ---
    CreateDepthStencil();

    /// --- PSO作成 ---
    CreatePSO(device,rtvFormat,dsvFormat);
}

void PSOManager::CreateRootSignature(ID3D12Device* device)
{
    // モデル
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

    rootSignatures_[(size_t)PrimitiveType::kModel] = rootSigBuilder.Build(device);

    // グリッド
    RootSignatureBuilder lineRootSigBuilder;
    lineRootSigBuilder.SetFlags(D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    D3D12_ROOT_PARAMETER lineRootParameters[1] = {};
    lineRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    lineRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
    lineRootParameters[0].Descriptor.ShaderRegister = 0;
    lineRootSigBuilder.AddRootParameter(lineRootParameters[0]);

    rootSignatures_[(size_t)PrimitiveType::kGrid] = lineRootSigBuilder.Build(device);
}

void PSOManager::CompileAllShaders() {
    for (int i = 0; i < static_cast<int>(PrimitiveType::kCount); ++i) {
        for (int j = 0; j < static_cast<int>(ShaderStage::kCount); ++j) {

            // パスとプロファイルを取得
            const wchar_t* path = kShaderPaths[i][j];
            const wchar_t* profile = kShaderProfiles[j];

            // コンパイルして配列に格納
            shaderBlobs_[i][j] = shaderCompiler_.CompileShader(path, profile);
            assert(shaderBlobs_[i][j] != nullptr && "Shader compilation failed!");
        }
    }
}

void PSOManager::CreateInputLayout()
{
    // モデル
    modelInputElementDescs_[0].SemanticName = "POSITION";
    modelInputElementDescs_[0].SemanticIndex = 0;
    modelInputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    modelInputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    modelInputElementDescs_[1].SemanticName = "TEXCOORD";
    modelInputElementDescs_[1].SemanticIndex = 0;
    modelInputElementDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    modelInputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    modelInputElementDescs_[2].SemanticName = "NORMAL";
    modelInputElementDescs_[2].SemanticIndex = 0;
    modelInputElementDescs_[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    modelInputElementDescs_[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    // ⭐ メンバ配列のアドレスとサイズを設定する
    inputLayoutDescs_[(size_t)PrimitiveType::kModel].pInputElementDescs = modelInputElementDescs_.data();
    inputLayoutDescs_[(size_t)PrimitiveType::kModel].NumElements = kModelInputElements;

    // グリッド
    gridInputElementDescs_[0].SemanticName = "POSITION";
    gridInputElementDescs_[0].SemanticIndex = 0;
    gridInputElementDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    gridInputElementDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
    gridInputElementDescs_[1].SemanticName = "TEXCOORD";
    gridInputElementDescs_[1].SemanticIndex = 0;
    gridInputElementDescs_[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    gridInputElementDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

    // ⭐ メンバ配列のアドレスとサイズを設定する
    inputLayoutDescs_[(size_t)PrimitiveType::kGrid].pInputElementDescs = gridInputElementDescs_.data();
    inputLayoutDescs_[(size_t)PrimitiveType::kGrid].NumElements = kGridInputElements;
}
void PSOManager::CreateDepthStencil() 
{
    // モデル
    depthStencilDescs_[(size_t)PrimitiveType::kModel].DepthEnable = true;
    depthStencilDescs_[(size_t)PrimitiveType::kModel].DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDescs_[(size_t)PrimitiveType::kModel].DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

    // グリッド
    depthStencilDescs_[(size_t)PrimitiveType::kGrid].DepthEnable = true;
    depthStencilDescs_[(size_t)PrimitiveType::kGrid].DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    depthStencilDescs_[(size_t)PrimitiveType::kGrid].DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
}

void PSOManager::CreatePSO(ID3D12Device* device, DXGI_FORMAT rtvFormat, DXGI_FORMAT dsvFormat)
{
    // モデル
    for (int i = 0; i < static_cast<int>(BlendMode::kCount); ++i) {
        BlendMode mode = static_cast<BlendMode>(i);
        GraphicsPipelineStateBuilder psoBuilder;
        graphicsPipelineStates_[(size_t)PrimitiveType::kModel][i] = psoBuilder
            .SetRootSignature(rootSignatures_[(size_t)PrimitiveType::kModel].Get())
            .SetInputLayout(inputLayoutDescs_[(size_t)PrimitiveType::kModel])
            .SetVertexShader(shaderBlobs_[(size_t)PrimitiveType::kModel][(size_t)ShaderStage::kVertex].Get())
            .SetPixelShader(shaderBlobs_[(size_t)PrimitiveType::kModel][(size_t)ShaderStage::kPixel].Get())
            .SetBlendMode(mode)
            .SetRasterizerState(rasterizerDesc_)
            .SetDepthStencilState(depthStencilDescs_[(size_t)PrimitiveType::kModel])
            .AddRenderTargetFormat(rtvFormat)
            .SetDepthStencilViewFormat(dsvFormat)
            .SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
            .Build(device);
    }

    GraphicsPipelineStateBuilder linePsoBuilder;
    graphicsPipelineStates_[(size_t)PrimitiveType::kGrid][(size_t)BlendMode::kNone] = linePsoBuilder
        .SetRootSignature(rootSignatures_[(size_t)PrimitiveType::kGrid].Get())
        .SetInputLayout(inputLayoutDescs_[(size_t)PrimitiveType::kGrid])
        .SetVertexShader(shaderBlobs_[(size_t)PrimitiveType::kGrid][(size_t)ShaderStage::kVertex].Get())
        .SetPixelShader(shaderBlobs_[(size_t)PrimitiveType::kGrid][(size_t)ShaderStage::kPixel].Get())
        .SetBlendMode(BlendMode::kNone)
        .SetRasterizerState(rasterizerDesc_)
        .SetDepthStencilState(depthStencilDescs_[(size_t)PrimitiveType::kGrid])
        .AddRenderTargetFormat(rtvFormat)
        .SetDepthStencilViewFormat(dsvFormat)
        .SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)
        .Build(device);
}