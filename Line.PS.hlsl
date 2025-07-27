struct Material {
    float4 color;
};

ConstantBuffer<Material> gMaterial : register(b0);

float4 main() : SV_TARGET {
    return gMaterial.color;
}