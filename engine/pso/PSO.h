#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <string>
#include <vector>
#include <dxcapi.h>

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

    // 線描画用のPSOを作成する関数を追加
    void CreateLinePSO(
        ID3D12Device* device,
        // LogStream& logStream, // LogStreamを引数として渡す場合
        DXGI_FORMAT rtvFormat,
        DXGI_FORMAT dsvFormat);

    // 線描画用のPSOを取得する関数を追加
    ID3D12PipelineState* GetLinePipelineState() { return linePipelineState_.Get(); }
    ID3D12RootSignature* GetLineRootSignature() { return lineRootSignature_.Get(); }

    // 作成されたオブジェクトのゲッター
    ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); }
    ID3D12PipelineState* GetPipelineState() const { return graphicsPipelineState_.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
    Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;

    Microsoft::WRL::ComPtr<ID3D12PipelineState> linePipelineState_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12RootSignature> lineRootSignature_ = nullptr;

    // シェーダーコンパイルのヘルパー関数 (staticにするか、ユーティリティに移動することも可能)
    Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
        const std::wstring& filePath,
        const wchar_t* profile,
        IDxcUtils* dxcUtils,
        IDxcCompiler3* dxcCompiler,
        IDxcIncludeHandler* includeHandler
    /*, LogStream& logStream // LogStreamを引数として渡す場合*/);
};