
#include "Line.h"
#include <cassert>
#include"../bonjin/BonjinEngine.h" 


Line::Line() {
}

Line::~Line() {
    if (vertexResource_ && vertexData_) {
        vertexResource_->Unmap(0, nullptr);
    }
    if (indexResource_ && indexData_) {
        indexResource_->Unmap(0, nullptr);
    }
    if (colorResource_ && colorData_) {
        colorResource_->Unmap(0, nullptr);
    }
    if (wvpResource_ && wvpData_) {
        wvpResource_->Unmap(0, nullptr);
    }
    if (materialResource_ && materialData_) {
        materialResource_->Unmap(0, nullptr);
    }
}

void Line::Initialize(uint32_t maxVertices) {
    maxVertices_ = maxVertices;
    maxIndices_ = maxVertices; // LINELISTの場合、頂点数とインデックス数は同じ

    ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();

    // 頂点バッファの生成
    vertexResource_ = CreateBufferResource(device, sizeof(VertexData) * maxVertices_);
    assert(vertexResource_);
    HRESULT hr = vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
    assert(SUCCEEDED(hr));

    // 頂点バッファビューの設定
    vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * maxVertices_;
    vertexBufferView_.StrideInBytes = sizeof(VertexData);
    currentVertexCount_ = 0;

    // インデックスバッファの生成 (LINELISTを使用するため、線ごとの2頂点インデックス)
    indexResource_ = CreateBufferResource(device, sizeof(uint32_t) * maxIndices_);
    assert(indexResource_);
    hr = indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&indexData_));
    assert(SUCCEEDED(hr));
    // インデックスバッファビューの設定
    indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * maxIndices_;
    indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
    currentIndexCount_ = 0;

    // マテリアルリソースの生成とマップ
    materialResource_ = CreateBufferResource(device, sizeof(Material));
    assert(materialResource_);
    hr = materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
    assert(SUCCEEDED(hr));
    materialData_->color = ToVector4(Color::White); // 初期色は白

    // WVPリソースの生成とマップ
    wvpResource_ = CreateBufferResource(device, sizeof(Matrix4x4));
    assert(wvpResource_);
    hr = wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData_));
    assert(SUCCEEDED(hr));
    wvpData_->WVP = MakeIdentity4x4(); // 初期化
    wvpData_->World = MakeIdentity4x4(); // 初期化
}

void Line::AddLine(const std::vector<Vector3>& points, const Color& color, const Camera& camera) {
    // 描画する頂点数がバッファの最大サイズを超えないかチェック
    if (currentVertexCount_ + points.size() > maxVertices_ || currentIndexCount_ + (points.size() - 1) * 2 > maxIndices_) {
        // バッファがいっぱいなら何もしないか、エラー処理を行う
        return;
    }

    // 頂点データをバッファにコピーし、インデックスを設定
    for (size_t i = 0; i < points.size(); ++i) {
        vertexData_[currentVertexCount_ + i] = { {points[i].x, points[i].y, points[i].z, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
        if (i > 0) {
            // 各線分は2つのインデックスを持つ
            indexData_[currentIndexCount_++] = currentVertexCount_ + uint32_t(i) - 1;
            indexData_[currentIndexCount_++] = currentVertexCount_ + uint32_t(i);
        }
    }
    currentVertexCount_ += static_cast<uint32_t>(points.size());

    // WVP行列を更新
    // ここではワールド行列は単位行列として、ビュープロジェクション行列を適用
    Matrix4x4 worldViewProjectionMatrix = Multiply(MakeIdentity4x4(), camera.GetViewProjectionMatrix());
    wvpData_->WVP = worldViewProjectionMatrix;

    // マテリアルの更新
    materialData_->color = ToVector4(color);
}


void Line::AddBezierCurve(
    const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3,
    int segments, const Color& color, const Camera& camera) {

    // ベジェ曲線は segments + 1 個の頂点から segments 個の線分を生成する
    // 各線分は2つの頂点インデックスを持つため、segments * 2 のインデックスが必要
    if (currentVertexCount_ + (segments + 1) > maxVertices_ || currentIndexCount_ + segments * 2 > maxIndices_) {
        // バッファがいっぱいなら何もしないか、エラー処理を行う
        return;
    }

    Vector3 prevPoint = p0;
    uint32_t initialVertexCount = currentVertexCount_; // この曲線の開始頂点インデックスを記録

    // 最初の点
    vertexData_[currentVertexCount_++] = { {p0.x, p0.y, p0.z, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

    for (int i = 1; i <= segments; ++i) {
        float t = static_cast<float>(i) / segments;

        // 3次ベジェ曲線の計算式
        Vector3 currentPoint = (
            Multiply(float(std::pow(1.0f - t, 3.0f)), p0) +
            Multiply(float(3.0f * std::pow(1.0f - t, 2.0f) * t), p1) +
            Multiply(float(3.0f * (1.0f - t) * std::pow(t, 2.0f)), p2) +
            Multiply(float(std::pow(t, 3.0f)), p3));

        // 線分を追加
        vertexData_[currentVertexCount_] = { {currentPoint.x, currentPoint.y, currentPoint.z, 1.0f}, {0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };
        indexData_[currentIndexCount_++] = currentVertexCount_ - 1; // prevPointのインデックス
        indexData_[currentIndexCount_++] = currentVertexCount_;       // currentPointのインデックス
        currentVertexCount_++;

        prevPoint = currentPoint;
    }

    // 色定数バッファを更新（最後のAddLine/AddBezierCurveで設定された色が全体の描画に適用されます）
    materialData_->color = ToVector4(color);

    // WVP行列を更新
    wvpData_->WVP = Multiply(MakeIdentity4x4(), camera.GetViewProjectionMatrix());
}


void Line::AddBezierPath(const std::vector<Vector3>& pathPoints, int segmentsPerCurve, const Color& color, const Camera& camera) {
    if (pathPoints.empty()) {
        return;
    }

    size_t i = 0;
    // フルな3次ベジェ曲線を処理
    for (; i + 3 < pathPoints.size(); i += 3) {
        // P0 P1 P2 P3
        const Vector3& p0 = pathPoints[i];
        const Vector3& p1 = pathPoints[i + 1];
        const Vector3& p2 = pathPoints[i + 2];
        const Vector3& p3 = pathPoints[i + 3];
        AddBezierCurve(p0, p1, p2, p3, segmentsPerCurve, color, camera);
    }

    // 残りの点を処理
    size_t remainingCount = pathPoints.size() - i;

    if (remainingCount == 2) {
        // 2点残っている場合 (P_i, P_{i+1}) -> 直線として描画
        std::vector<Vector3> linePoints = { pathPoints[i], pathPoints[i + 1] };
        AddLine(linePoints, color, camera);
    } else if (remainingCount == 3) {
        // 3点残っている場合 (P_i, P_{i+1}, P_{i+2}) -> 2次ベジェ曲線として描画
        // 2次ベジェ曲線 (p0, p1, p2) を3次ベジェ曲線 (b0, b1, b2, b3) に変換
        const Vector3& p0 = pathPoints[i];
        const Vector3& p1 = pathPoints[i + 1];
        const Vector3& p2 = pathPoints[i + 2];

        Vector3 b0 = p0;
        Vector3 b1 = Add(p0, Multiply(2.0f / 3.0f, Subtract(p1, p0)));
        Vector3 b2 = Add(p2, Multiply(2.0f / 3.0f, Subtract(p1, p2)));
        Vector3 b3 = p2;

        AddBezierCurve(b0, b1, b2, b3, segmentsPerCurve, color, camera);
    }
    // remainingCount が 0 または 1 の場合は何もしない（描画可能な線分がないため）
}
void Line::Clear() {
    currentVertexCount_ = 0;
    currentIndexCount_ = 0;
}

void Line::Draw() {
    ID3D12GraphicsCommandList* commandList = DirectXCommon::GetInstance()->GetCommandList();
    PSO* pso = DirectXCommon::GetInstance()->GetPSO(); // PSOインスタンスを取得

    // ★線描画用のルートシグネチャとPSOを設定★
    commandList->SetGraphicsRootSignature(pso->GetLineRootSignature());
    commandList->SetPipelineState(pso->GetLinePipelineState());

    // 頂点バッファビューのサイズを現在描画する頂点数に合わせて設定
    vertexBufferView_.SizeInBytes = sizeof(VertexData) * currentVertexCount_;
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

    // インデックスバッファビューのサイズを現在描画するインデックス数に合わせて設定
    indexBufferView_.SizeInBytes = sizeof(uint32_t) * currentIndexCount_;
    commandList->IASetIndexBuffer(&indexBufferView_);

    // プリミティブトポロジの設定 (線リスト)
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);

    // マテリアルCBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
    // WVP CBufferの場所を設定
    commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

    // 描画コマンド
    commandList->DrawIndexedInstanced(currentIndexCount_, 1, 0, 0, 0);
}