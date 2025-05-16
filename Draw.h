#pragma once
#include"d3d12.h"
#include<vector>
#include"Convert.h"


class DrawTriangle {
private:

	ID3D12GraphicsCommandList* commandList_{};
	ID3D12Device* device_{};
	ID3D12PipelineState* graphicsPipelineState_{};
	ID3D12RootSignature* rootSignature_ = nullptr;


	ID3D12Resource* vertexResource_{};
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	ID3D12Resource* materialResource_{};
	Vector4* materialData_ = {};
	ID3D12Resource* wvpResource_{};
	Matrix4x4* wvpData_ = {};
	D3D12_RECT scissorRect_{};
	D3D12_VIEWPORT viewport_{};



	//クライアント領域のサイズ
	const int kClientWidth = 1280;
	const int kClientHeight = 720;


public:

	void Initialize(ID3D12RootSignature* rootSignature, ID3D12PipelineState* graphicsPipelineState, ID3D12GraphicsCommandList* commandList, ID3D12Device* device);

	void PreDraw(float x1, float y1, float x2, float y2, float x3, float y3);

	void Update(Transform transform, ID3D12Resource* materialResource);

	void Finalize();
};

