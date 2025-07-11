// グリッド描画用のピクセルシェーダー
// Grid.hlsl (仮称)

// 頂点シェーダーから受け取る構造体
struct VSOutput
{
    float4 position : SV_POSITION;
    float4 worldPos : TEXCOORD0; // ワールド座標を頂点シェーダーから受け取る
};

// ピクセルシェーダーのエントリーポイント
float4 main(VSOutput input) : SV_TARGET
{
    // グリッドのワールド座標を取得
    float x = input.worldPos.x;
    float z = input.worldPos.z;

    // 5mごとの色分けロジック
    // fmod(x, 5.0f) は x を 5.0 で割った余りを計算します。
    // その値がしきい値に近い場合 (すなわち、5mの境界線に近い場合) に色を変えます。
    float thickness = 0.08f; // ★太い線の幅 (メートル単位)。0.05fより少し太くしてみる
    float originThickness = 0.1f; // ★原点線の太さ

    // X軸方向の5mごとのライン判定
    bool isMajorXLine = (fmod(abs(x), 5.0f) < thickness) || (fmod(abs(x), 5.0f) > (5.0f - thickness));
    // Z軸方向の5mごとのライン判定
    bool isMajorZLine = (fmod(abs(z), 5.0f) < thickness) || (fmod(abs(z), 5.0f) > (5.0f - thickness));

    // 原点 (0,0,0) のX軸またはZ軸のラインかを判定
    bool isOriginXLine = (abs(z) < originThickness); // Z=0に近い線はX軸
    bool isOriginZLine = (abs(x) < originThickness); // X=0に近い線はZ軸

    // 色の定義 (RGB, 各成分は 0.0～1.0)
    float4 normalGridColor = float4(0.1f, 0.1f, 0.1f, 1.0f); // ★標準のグリッド線 (非常に暗い灰色)
    float4 majorGridColor = float4(0.3f, 0.3f, 0.3f, 1.0f); // ★5mごとの線 (やや明るい青みがかった灰色)
    float4 originLineColor = float4(0.8f, 0.8f, 0.8f, 1.0f); // ★原点線 (明るい灰色、ほぼ白)

    // X軸とZ軸の主線に色を適用
    if (isMajorXLine || isMajorZLine)
    {
        // 原点線が5mラインと重なる場合、原点線を優先
        if ((abs(x) < originThickness && isMajorZLine) || (abs(z) < originThickness && isMajorXLine))
        {
            return originLineColor; // 原点線
        }
        return majorGridColor; // 5mごとのライン
    }
    
    // 原点線だが、5mラインではない場合 (例: 原点線が5mラインと重ならない領域にある場合)
    // ただし、このグリッドは5mラインが必ず原点を通るので、上記のisMajorXLine || isMajorZLineの条件でカバーされることが多い
    // 安全のため残すが、実質的には上のifブロックで処理される可能性が高い
    if (isOriginXLine || isOriginZLine)
    {
        return originLineColor; // 原点線
    }

    // それ以外の線は薄い灰色
    return normalGridColor; // 標準のグリッド線
}