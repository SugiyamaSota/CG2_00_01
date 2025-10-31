#include "Particle.h"

Particle::Particle() {
	common = DirectXCommon::GetInstance();
	transform_ = InitializeWorldTransform();
	viewMatrix_ = MakeIdentity4x4();
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
	viewProjectionMatrix_ = MakeIdentity4x4();
}

void Particle::LoadModel(const std::string& fileName) {
	// モデルファイル読み込み
	modelData_ = ModelBuilder::LoadObjFile("resources/models/" + fileName, fileName + ".obj");

	// 頂点用のリソース
	vertexResource_ = CreateBufferResource(common->GetDevice(), sizeof(VertexData) * modelData_.vertices.size());
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	// マテリアル用のリソース
	materialResource_ = CreateBufferResource(common->GetDevice(), sizeof(Material));
	materialData_ = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = false;
	materialData_->uvTransform = MakeIdentity4x4();

	// WVP用のリソース
	wvpResource_ = CreateBufferResource(common->GetDevice(), sizeof(TransformationMatrix) * kNumInstance_);
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));

	for (uint32_t index = 0; index < kNumInstance_; ++index) 
	{
		wvpData_[index].WVP = MakeIdentity4x4();
		wvpData_[index].World = MakeIdentity4x4();
	}

	// テクスチャ
	textureHandle_ = TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilepath);
	common->WaitAndResetCommandList();
	TextureManager::GetInstance()->ReleaseIntermediateResources();

	
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = kNumInstance_;
	srvDesc.Buffer.StructureByteStride = sizeof(TransformationMatrix);
	srvhandleCPU_ = GetCPUDescriptorHandle(DirectXCommon::GetInstance()->GetSRVDescriptorHeap(), DirectXCommon::GetInstance()->GetSRVSize(), 3);
	srvhandleGPU_ = GetGPUDescriptorHandle(DirectXCommon::GetInstance()->GetSRVDescriptorHeap(), DirectXCommon::GetInstance()->GetSRVSize(), 3);
	common->GetDevice()->CreateShaderResourceView(wvpResource_.Get(), &srvDesc, srvhandleCPU_);
}

void Particle::Update(WorldTransform worldTransform, Camera* camera) {
	transform_ = worldTransform;
	// ワールドトランスフォーム
	for (uint32_t index = 0; index < kNumInstance_; ++index)
	{
		wvpData_[index].World = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
		Matrix4x4 worldViewProjectionMatrix = Multiply(wvpData_[index].World, camera->GetViewProjectionMatrix());
		wvpData_[index].WVP = worldViewProjectionMatrix;
		transform_.translate = { index * 0.1f,index * 0.1f,index * 0.1f };
	}
}

void Particle::Draw() {
	// PSOの設定
	common->GetCommandList()->SetGraphicsRootSignature(common->GetPSO()->GetRootSignature(PrimitiveType::kParticle));
	common->GetCommandList()->SetPipelineState(common->GetPSO()->GetPipelineState(PrimitiveType::kParticle, BlendMode::kNormal));

	//　モデルの描画
	// VBV
	common->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	// 形状を設定
	common->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// マテリアルCBufferの場所を設定
	common->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	common->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvhandleGPU_);

	// SRV用のdescriptionTavleの先頭を設定
	common->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetGPUHandle(textureHandle_));
	// 光
	common->GetCommandList()->SetGraphicsRootConstantBufferView(3, common->GetDirectionalLightResource()->GetGPUVirtualAddress());
	// 描画
	common->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), kNumInstance_, 0, 0);
}

