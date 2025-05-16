#include "Draw.h"
#include"Function.h"
#include"externals/imgui/imgui.h"

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
};

void DrawTriangle::Initialize(ID3D12RootSignature* rootSignature, ID3D12PipelineState* graphicsPipelineState, ID3D12GraphicsCommandList* commandList, ID3D12Device* device) {
	commandList_ = commandList;
	device_ = device;
	graphicsPipelineState_ = graphicsPipelineState;
	rootSignature_ = rootSignature;
}

void DrawTriangle::PreDraw(float x1, float y1, float x2, float y2, float x3, float y3) {
	///// 頂点リソース作成 /////
	vertexResource_ = CreateBufferResource(device_, sizeof(VertexData) * 6);
	//頂点バッファビューを作成する
	//リソース先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	//1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = CreateBufferResource(device_, sizeof(Vector4));

	//WVP用のリソースを作る
	wvpResource_ = CreateBufferResource(device_, sizeof(Matrix4x4));
	//マテリアルにデータを書き込む
	materialData_ = nullptr;
	//書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	//今回は赤を書き込む
	*materialData_ = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
	//データを書き込む
	wvpData_ = nullptr;
	//書き込むためのアドレスを取得
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	//単位行列を書き込んでおく
	*wvpData_ = MakeIdentity4x4();

	///// 頂点リソースにデータを書き込む /////
	VertexData* vertexData_ = nullptr;
	//書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

	vertexData_[0].position = { x1,y1,0.0f,1.0f };
	vertexData_[0].texcoord = { 0.0f,1.0f };
	vertexData_[1].position = { x2,y2,0.0f,1.0f };
	vertexData_[1].texcoord = { 0.5f,0.0f };
	vertexData_[2].position = { x3,y3,0.0f,1.0f };
	vertexData_[2].texcoord = { 1.0f,1.0f };

	vertexData_[3].position = { x3,y3,0.0f,0.9f };
	vertexData_[3].texcoord = { 0.0f,1.0f };
	vertexData_[4].position = { x2,y2,0.0f,0.9f };
	vertexData_[4].texcoord = { 0.5f,0.0f };
	vertexData_[5].position = { x1,y1,0.0f,0.9f };
	vertexData_[5].texcoord = { 1.0f,1.0f };


	//ビューポート
	viewport_.Width = float(kClientWidth);
	viewport_.Height = float(kClientHeight);
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	//シザー矩形
	scissorRect_.left = 0;
	scissorRect_.right = kClientWidth;
	scissorRect_.top = 0;
	scissorRect_.bottom = kClientHeight;
}

void DrawTriangle::Update(Transform transform, ID3D12Resource* materialResource) {

	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	*wvpData_ = worldMatrix;

	commandList_->RSSetViewports(1, &viewport_);
	commandList_->RSSetScissorRects(1, &scissorRect_);
	commandList_->SetGraphicsRootSignature(rootSignature_);
	commandList_->SetPipelineState(graphicsPipelineState_);
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	commandList_->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	commandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	commandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList_->DrawInstanced(6, 1, 0, 0);
}

void DrawTriangle::Finalize() {
	vertexResource_->Release();
	materialResource_->Release();
	wvpResource_->Release();
}