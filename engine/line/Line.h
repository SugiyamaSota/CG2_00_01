#pragma once
#include <d3d12.h>
#include <wrl/client.h>
#include <vector>
#include "../math/Struct.h"
#include "../color/Color.h"

class Camera; // Cameraクラスの前方宣言

class Line {
public:
    Line();
    ~Line();

    /// <summary>
    /// 線の初期化
    /// </summary>
    /// <param name="maxVertices">確保する頂点バッファの最大頂点数</param>
    void Initialize(uint32_t maxVertices);

    /// <summary>
    /// 線分を内部バッファに追加
    /// </summary>
    /// <param name="points">線分の頂点リスト</param>
    /// <param name="color">線の色</param>
    /// <param name="camera">カメラ情報</param>
    void AddLine(const std::vector<Vector3>& points, const Color& color, const Camera& camera);

    /// <summary>
    /// 3次ベジェ曲線から生成される線分を内部バッファに追加
    /// </summary>
    /// <param name="p0">制御点0</param>
    /// <param name="p1">制御点1</param>
    /// <param name="p2">制御点2</param>
    /// <param name="p3">制御点3</param>
    /// <param name="segments">分割数</param>
    /// <param name="color">線の色</param>
    /// <param name="camera">カメラ情報</param>
    void AddBezierCurve(
        const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3,
        int segments, const Color& color, const Camera& camera);

    /// <summary>
    /// 複数の制御点から連続した3次ベジェ曲線のパスを内部バッファに追加
    /// </summary>
    /// <param name="pathPoints">ベジェ曲線パスを構成するすべての制御点リスト</param>
    /// <param name="segmentsPerCurve">各ベジェ曲線の分割数</param>
    /// <param name="color">線の色</param>
    /// <param name="camera">カメラ情報</param>
    void AddBezierPath(const std::vector<Vector3>& pathPoints, int segmentsPerCurve, const Color& color, const Camera& camera);

    /// <summary>
    /// 指定されたt値でベジェ曲線パス上の点を計算
    /// </summary>
    /// <param name="pathPoints">ベジェ曲線パスを構成するすべての制御点リスト</param>
    /// <param name="t">パス全体にわたるパラメータ (0.0 から 1.0)</param>
    /// <returns>計算された点の座標</returns>
    Vector3 CalculateBezierPoint(const std::vector<Vector3>& pathPoints, float t);

    /// <summary>
    /// ベジェ曲線パスの近似長さを計算
    /// </summary>
    /// <param name="pathPoints">ベジェ曲線パスを構成するすべての制御点リスト</param>
    /// <param name="segmentsPerCurve">各ベジェ曲線の分割数 (長さを概算する際の精度)</param>
    /// <returns>ベジェ曲線パスの近似長さ</returns>
    float GetBezierPathLength(const std::vector<Vector3>& pathPoints, int segmentsPerCurve = 30);


    /// <summary>
    /// 内部バッファをクリア
    /// </summary>
    void Clear();

    /// <summary>
    /// 内部バッファに蓄積された線分を全て描画
    /// </summary>
    void Draw();

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
    VertexData* vertexData_ = nullptr; // 頂点バッファがマップされるポインタ
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    uint32_t maxVertices_ = 0;        // 確保した頂点バッファの最大頂点数
    uint32_t currentVertexCount_ = 0; // 現在描画する頂点数

    Microsoft::WRL::ComPtr<ID3D12Resource> colorResource_ = nullptr;
    Color* colorData_ = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource_ = nullptr;
    TransformationMatrix* wvpData_ = nullptr; // WVP 行列がマップされるポインタ

    // インデックスバッファ関連 (LINELIST用)
    Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;
    uint32_t* indexData_ = nullptr;
    D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
    uint32_t maxIndices_ = 0;
    uint32_t currentIndexCount_ = 0;

    // 行列
    Matrix4x4 viewMatrix_;
    Matrix4x4 projectionMatrix_;
    Matrix4x4 viewProjectionMatrix_;

    Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_ = nullptr;
    Material* materialData_ = nullptr;

    // Helper function for cubic bezier point
    Vector3 CalculateCubicBezierPoint(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Vector3& p3, float t);
    // Helper function for quadratic bezier point
    Vector3 CalculateQuadraticBezierPoint(const Vector3& p0, const Vector3& p1, const Vector3& p2, float t);
};