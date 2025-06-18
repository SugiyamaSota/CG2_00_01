#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <string>
#include <vector>
#include <dxcapi.h>

// 循環参照を避けるための前方宣言 (PSOがこれらの型と相互作用する必要がある場合)
// あなたのログストリームの型に合わせてください
// struct LogStream; // 例: LogStreamがカスタム型の場合
// class DirectXCommon; // 例: DirectXCommonがPSO内部で必要だが循環参照になる場合

class PSO {
public:
    PSO();
    ~PSO();

    // PSOを初期化し、ルートシグネチャとパイプラインステートを作成します
    void Initialize(
        ID3D12Device* device,
        // LogStream& logStream, // LogStreamを引数として渡す場合
        DXGI_FORMAT rtvFormat,
        DXGI_FORMAT dsvFormat);

    // 作成されたオブジェクトのゲッター
    ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }
    ID3D12PipelineState* GetPipelineState() const { return graphicsPipelineState_.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

    // シェーダーコンパイルのヘルパー関数 (staticにするか、ユーティリティに移動することも可能)
    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
        const std::wstring& filePath,
        const wchar_t* profile,
        IDxcUtils* dxcUtils,
        IDxcCompiler3* dxcCompiler,
        IDxcIncludeHandler* includeHandler
    /*, LogStream& logStream // LogStreamを引数として渡す場合*/);
};