#include "LineRenderer.h"
#include <cassert>
#include <iostream>

extern Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(ID3D12Device* device, size_t sizeInBytes); // main.cppから移動または共通ヘッダに定義

LineRenderer::LineRenderer() :
	transformationMatrixData_(nullptr)
{
}

LineRenderer::~LineRenderer() {
}

void LineRenderer::Initialize() {
	// 定数バッファの作成
	transformationMatrixResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(LineTransformationMatrix));
	HRESULT hr = transformationMatrixResource_->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData_));
	assert(SUCCEEDED(hr));
}

void LineRenderer::DrawLine(const Vector4& start, const Vector4& end, const Vector4& color, const Matrix4x4& viewProjectionMatrix) {
	ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

	lineVertices_.clear();
	lineVertices_.push_back({ start, color });
	lineVertices_.push_back({ end, color });

	// 頂点バッファの作成とデータ転送
	if (vertexBuffer_ == nullptr || vertexBuffer_->GetDesc().Width < sizeof(LineVertex) * lineVertices_.size()) {
		vertexBuffer_ = CreateBufferResource(device, sizeof(LineVertex) * lineVertices_.size());
	}
	LineVertex* mappedVertices = nullptr;
	HRESULT hr = vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertices));
	assert(SUCCEEDED(hr));
	std::copy(lineVertices_.begin(), lineVertices_.end(), mappedVertices);
	vertexBuffer_->Unmap(0, nullptr);

	vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	vbView_.SizeInBytes = static_cast<UINT>(sizeof(LineVertex) * lineVertices_.size());
	vbView_.StrideInBytes = sizeof(LineVertex);

	transformationMatrixData_->worldviewProjection = viewProjectionMatrix;

	commandList->SetGraphicsRootSignature(DirectXCommon::GetInstance()->GetPSO()->GetLineRootSignature()); // LineRenderer用のルートシグネチャを設定
	commandList->SetPipelineState(DirectXCommon::GetInstance()->GetPSO()->GetLinePipelineState()); // LineRenderer用のパイプラインステートを設定
	commandList->IASetVertexBuffers(0, 1, &vbView_);
	commandList->SetGraphicsRootConstantBufferView(0, transformationMatrixResource_->GetGPUVirtualAddress());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	commandList->DrawInstanced(static_cast<UINT>(lineVertices_.size()), 1, 0, 0);
}

void LineRenderer::Finalize() {
	// リソースの解放はComPtrが自動で行う
}