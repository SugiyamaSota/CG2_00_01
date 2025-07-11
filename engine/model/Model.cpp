#include "Model.h"
#include"../common/DirectXCommon.h"
#include <filesystem>
#include <fstream>
#include<cassert>
#include"../function/Utility.h"

#include"../math/Convert.h"
#include"../math/Matrix.h"

#include"../camera/Camera.h"
#include"../texture/TextureManager.h"

Model::Model(bool debugEnabled, const std::string& modelName) {
	transform_ = InitializeWorldTransform();
	viewMatrix_ = MakeIdentity4x4();
	projectionMatrix_ = MakePerspectiveFovMatrix(0.45f, float(1280) / float(720), 0.1f, 100.0f);
	viewProjectionMatrix_ = MakeIdentity4x4();
	debugEnabled_ = debugEnabled;
	if (debugEnabled) {
		// デバッグが有効
		if (!modelName.empty()) {
			// 名前を代入
			modelName_ = modelName;
		} else {
			// modelNameがnullなら既定の名前を代入
			modelName_ = "UnnamedModel_Debug";
		}
	} else {
		// デバッグが無効
		modelName_ = "";
	}
}

void Model::LoadModel(const std::string& fileName) {
	// モデル読み込み
	modelData_ = LoadObjFile("resources/models/" + fileName, fileName + ".obj");
	// 頂点用のリソース
	vertexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * modelData_.vertices.size());
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * modelData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	// マテリアル用のリソース
	materialResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material));
	materialData_ = nullptr;
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
	materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData_->enableLighting = true;
	materialData_->uvTransform = MakeIdentity4x4();

	// WVP用のリソース
	wvpResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix));
	wvpData_ = nullptr;
	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
	wvpData_->WVP = MakeIdentity4x4();
	wvpData_->World = MakeIdentity4x4();

	textureHandle_ = TextureManager::GetInstance()->LoadTexture(modelData_.material.textureFilepath);
	DirectXCommon::GetInstance()->WaitAndResetCommandList();
	TextureManager::GetInstance()->ReleaseIntermediateResources();
}

void Model::Initialize(WorldTransform worldTransform) {
	transform_ = worldTransform;
}

void Model::Update(WorldTransform worldTransform, Camera* camera, bool enableLighting, Vector4 color) {
	// デバッグを使用しない場合
	if (!debugEnabled_) {
		transform_ = worldTransform;
		materialData_->enableLighting = enableLighting;
		materialData_->color = color;
	}

	// デバッグを使用する場合
	if (debugEnabled_) {
		DrawImGui();
	}

	wvpData_->World = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);
	viewMatrix_ = Inverse(wvpData_->World);
	if (camera == nullptr) {
		viewProjectionMatrix_ = Multiply(viewMatrix_, projectionMatrix_);
		Matrix4x4 worldViewProjectionMatrix = Multiply(wvpData_->World, viewProjectionMatrix_);
		wvpData_->WVP = worldViewProjectionMatrix;
	} else {
		Matrix4x4 worldViewProjectionMatrix = Multiply(wvpData_->World, camera->GetViewProjectionMatrix());
		wvpData_->WVP = worldViewProjectionMatrix;
	}
}

void Model::Draw() {
	//モデルの描画
	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);//VBVを設定
	//形状を設定。PSOに設定しているものとは別。同じものを設定るすると考える
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//SRVようのdescriptionTavleの先頭を設定
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetGPUHandle(textureHandle_));
	//描画!3頂点で1つのインスタンス
	DirectXCommon::GetInstance()->GetCommandList()->DrawInstanced(UINT(modelData_.vertices.size()), 1, 0, 0);
}

//mtlファイルの読み込み
MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + '/' + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilepath = directoryPath + '/' + textureFilename;
		}
	}
	return materialData;
}

//objファイルの読み込み
ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData;
	std::vector<Vector4>positions;
	std::vector<Vector3>normals;
	std::vector<Vector2>texcoords;
	std::string line;

	std::ifstream file(directoryPath + '/' + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.x *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {
			VertexData triangle[3];
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position,texcoord,normal };
			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			std::string materialFilename;
			s >> materialFilename;

			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}

void Model::DrawImGui() {
	if (debugEnabled_ == true) {
		ImGui::Begin(modelName_.c_str());
		if (ImGui::CollapsingHeader("wvpData")) {
			// スケール
			std::string scaleLabel = modelName_ + ": scale";
			ImGui::DragFloat3(scaleLabel.c_str(), &transform_.scale.x, 1.0f, -100.0f, 100.0f);

			// 回転
			std::string rotateLabel = modelName_ + ": rotate";
			ImGui::DragFloat3(rotateLabel.c_str(), &transform_.rotate.x, 1.0f, -100.0f, 100.0f);

			// 移動
			std::string translateLabel = modelName_ + ": translate";
			ImGui::DragFloat3(translateLabel.c_str(), &transform_.translate.x, 1.0f, -100.0f, 100.0f);
		}
		if (ImGui::CollapsingHeader("materialData")) {
			// 色
			std::string colorLabel = modelName_ + ": color";
			ImGui::ColorEdit4(colorLabel.c_str(), &materialData_->color.x);

			// ライティング
			std::string enableLightingLabel = modelName_ + " : Enable Lighting";

			bool isLightingEnabled = (materialData_->enableLighting != 0);

			if (ImGui::Checkbox(enableLightingLabel.c_str(), &isLightingEnabled)) {
				materialData_->enableLighting = isLightingEnabled ? 1 : 0;
			}
		}
		ImGui::End();
	}
}

