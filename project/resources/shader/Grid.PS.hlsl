struct VSOutput
{
    float4 position : SV_POSITION;
    float4 worldPos : TEXCOORD0;
};

float4 main(VSOutput input) : SV_TARGET
{
    float x = input.worldPos.x;
    float z = input.worldPos.z;

    
    float thickness = 0.08f; // 他より太い線
    float originThickness = 0.1f; // 原点線
    
    // 原点 (0,0,0) のX軸またはZ軸のラインかを判定
    bool isOriginXLine = (abs(z) < originThickness);
    bool isOriginZLine = (abs(x) < originThickness);
    
    // 強調する線の間隔(m単位)
    const float kHighLightInterval = 10.0f;
    
    // 強調線の判定
    bool isMajorXLine = (fmod(abs(x), kHighLightInterval) < thickness) || (fmod(abs(x), kHighLightInterval) > (kHighLightInterval - thickness));
    // Z軸方向の5mごとのライン判定
    bool isMajorZLine = (fmod(abs(z), kHighLightInterval) < thickness) || (fmod(abs(z), kHighLightInterval) > (kHighLightInterval - thickness));

    
    // 色の定義 (RGB, 各成分は 0.0～1.0)
    float4 normalGridColor = float4(0.1f, 0.1f, 0.1f, 1.0f); // 標準の線
    float4 majorGridColor = float4(0.3f, 0.3f, 0.3f, 1.0f); // 強調する線
    float4 originXLineColor = float4(0.0f, 1.0f, 0.0f, 1.0f); // 原点線X
    float4 originZLineColor = float4(1.0f, 0.0f, 0.0f, 1.0f); // 原点線Z
    
    
    // 原点線
    if (isOriginXLine)
    {
        return originXLineColor;
    }
    if (isOriginZLine)
    {
        return originZLineColor;
    }
    
    // 強調線
    if (isMajorXLine || isMajorZLine)
    {
        return majorGridColor;
    }
    
    // そのほか
    return normalGridColor;
}