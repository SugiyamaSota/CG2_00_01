#include "Sprite.h"
#include <cassert>

Sprite::Sprite() {}

Sprite::~Sprite() {
    // リソースは ComPtr によって管理されるため、自動的に解放されます。
    // ただし、マップされている場合はアンマップすることが良い習慣です。
    if (vertexResource_ && vertexData_) {
        vertexResource_->Unmap(0, nullptr);
    }
    if (materialResource_ && materialData_) {
        materialResource_->Unmap(0, nullptr);
    }
    if (wvpResource_ && wvpData_) {
        wvpResource_->Unmap(0, nullptr);
    }
    if (indexResource_ && indexData_) {
        indexResource_->Unmap(0, nullptr);
    }
}


void Sprite::Initialize(WorldTransform worldTransform, const std::string& textureFilePath) {
    transform_ = worldTransform;

    // 頂点バッファの生成
    vertexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * 4);
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * 4;
    vertexBufferView_.StrideInBytes = sizeof(VertexData);
    vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));

    // 頂点データの定義 (正方形)
   //左下
    vertexData_[0].position = { 0.0f,360.0f,0.0f,1.0f };
    vertexData_[0].texcoord = { 0.0f,1.0f };
    vertexData_[0].normal = { 0.0f,0.0f,1.0f };
    //左上
    vertexData_[1].position = { 0.0f,0.0f,0.0f,1.0f };
    vertexData_[1].texcoord = { 0.0f,0.0f };
    vertexData_[1].normal = { 0.0f,0.0f,1.0f };
    //右下
    vertexData_[2].position = { 640.0f,360.0f,0.0f,1.0f };
    vertexData_[2].texcoord = { 1.0f,1.0f };
    vertexData_[2].normal = { 0.0f,0.0f,1.0f };
    //右上
    vertexData_[3].position = { 640.0f,0.0f,0.0f,1.0f };
    vertexData_[3].texcoord = { 1.0f,0.0f };
    vertexData_[3].normal = { 0.0f,0.0f,1.0f };

    // インデックスバッファの生成
    indexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(uint32_t) * 6);
    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * 6;
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
    indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));

    // インデックスデータの定義 (2つの三角形で正方形を構成)
    indexData_[0] = 0;
    indexData_[1] = 1;
    indexData_[2] = 2;
    indexData_[3] = 1;
    indexData_[4] = 3;
    indexData_[5] = 2;

    // マテリアルリソースの生成
    materialResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material));
    materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
    materialData_->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f); // デフォルト色を白に設定
    materialData_->enableLighting = false; // スプライトはライティングを無効にする
    materialData_->uvTransform = MakeIdentity4x4(); // UV変換を初期化

    // WVP リソースの生成
    wvpResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(TransformationMatrix));
    wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
    wvpData_->WVP = MakeIdentity4x4(); // WVP 行列を初期化
    wvpData_->World = MakeIdentity4x4(); // ワールド行列を初期化

    // テクスチャのロード
    textureHandle_ = TextureManager::GetInstance()->LoadTexture(textureFilePath);
}

void Sprite::Update(WorldTransform worldTransform, Vector4 color, const Matrix4x4& viewMatrix, const Matrix4x4& projectionMatrix) {
    transform_ = worldTransform;

    // ワールド行列の計算
    Matrix4x4 worldMatrix = MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

    // WVP 行列の計算
    // スプライトは通常2Dなので、ビュー行列とプロジェクション行列は正射影を使用します。
    // main.cpp の例を参考に、ビュー行列は単位行列、プロジェクション行列は MakeOrthographicMatrix を使用してください。
    Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
    wvpData_->WVP = worldViewProjectionMatrix;

    // マテリアルの更新
    materialData_->color = color;

    // UV 変換行列の更新 (必要に応じて)
    // 例えば、スプライトシートアニメーションなどを行う場合に、uvTransform を更新します。
    Matrix4x4 uvTransformMatrix = MakeScaleMatrix(transform_.scale); // スプライトのスケールをUV変換に適用
    uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(transform_.rotate.z)); // Z軸回転をUV変換に適用
    uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix({ transform_.translate.x, transform_.translate.y, 0.0f })); // 平行移動をUV変換に適用
    materialData_->uvTransform = uvTransformMatrix;
}

void Sprite::Draw() {
    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();

    // インデックスバッファビューの設定
    commandList->IASetIndexBuffer(&indexBufferView_);
    // 頂点バッファビューの設定
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
    // プリミティブトポロジの設定 (三角形リスト)
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // マテリアルCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    // WVP CBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
    // テクスチャのDescriptorTableを設定
    commandList->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetGPUHandle(textureHandle_));

    // 描画コマンド
    commandList->DrawIndexedInstanced(6, 1, 0, 0, 0); // 6つのインデックス (2つの三角形) を描画
}