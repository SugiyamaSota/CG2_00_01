// グリッド描画用のシンプルな頂点シェーダー
// Grid.hlsl (仮称)

// 入力レイアウトに合わせる構造体
struct VSInput
{
    float4 position : POSITION0;
};

// ピクセルシェーダーに渡す構造体
struct VSOutput
{
    float4 position : SV_POSITION;
    float4 worldPos : TEXCOORD0; // ★追加
};

// グローバル定数バッファ (C++から渡す行列)
cbuffer TransformationMatrix : register(b0)
{
    float4x4 worldviewProjection;
};

// 頂点シェーダーのエントリーポイント
VSOutput main(VSInput input)
{
    VSOutput output;
    
    // ワールド変換を考慮しない場合、positionがそのままworldPosになる
    // もしグリッドに独自のワールド変換がある場合、ここでinput.positionにその行列を掛ける
    // output.worldPos = mul(input.position, WorldMatrix); のように
    output.worldPos = input.position; // そのままワールド座標として渡す

    // ワールドビュープロジェクション変換
    output.position = mul(input.position, worldviewProjection);

    return output;
}