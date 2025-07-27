// Line.VS.hlsl
struct VertexData
{
    float4 position : POSITION;
};

struct TransformationMatrix
{
    float4x4 WVP;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b1);

struct VSOutput
{
    float4 position : SV_POSITION;
};

VSOutput main(VertexData input)
{
    VSOutput output;
    output.position = mul(input.position, gTransformationMatrix.WVP);
    return output;
}