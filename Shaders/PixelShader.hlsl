//
// Pixel Shader
//

cbuffer psCB : register(b1)
{
    matrix lightPosition;
    matrix viewProjection;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 color : COLOR0;
};

//
// Pixel shader entrypoint
//
float4 main(PS_INPUT i) : SV_TARGET
{
    return float4(i.color, 1.0f);
}