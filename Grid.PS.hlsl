struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct PS_OUTPUT
{
    float4 target : SV_TARGET;
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT output;
    
    output.target = input.color;
    
    return output;
}