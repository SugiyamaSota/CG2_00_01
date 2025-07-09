cbuffer TransformationMatrix : register(b0)
{
    matrix worldviewProjection;
};

struct VS_INPUT
{
    float4 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    output.position = mul(input.position, worldviewProjection);
    
    output.color = float4(0.5f, 0.5f, 0.5f, 1.0f);
    
    return output;
}