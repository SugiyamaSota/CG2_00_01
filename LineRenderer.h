#pragma once
#include <vector>
#include <DirectXMath.h>
#include <wrl.h>
#include <d3d12.h>
#include"engine/bonjin/BonjinEngine.h"

// 頂点構造体
struct LineVertex {
	Vector4 position;
	Vector4 color; // 色を追加
};

// ワールドビュープロジェクション行列用の定数バッファ
struct LineTransformationMatrix {
	Matrix4x4 worldviewProjection;
};

class LineRenderer {
public:
	LineRenderer();
	~LineRenderer();

	void Initialize();
	void DrawLine(const Vector4& start, const Vector4& end, const Vector4& color, const Matrix4x4& viewProjectionMatrix);
	void Finalize();

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
	D3D12_VERTEX_BUFFER_VIEW vbView_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResource_;
	LineTransformationMatrix* transformationMatrixData_;

	std::vector<LineVertex> lineVertices_; // 描画する線分の頂点データを保持

	void CreatePipeline();
	// 現在のPSOをGridからLineRenderer用に差し替える
	// ID3D12RootSignature* rootSignature_; 
	// ID3D12PipelineState* pipelineState_;
};