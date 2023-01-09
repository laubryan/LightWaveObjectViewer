//
// Pixel Shader
//

cbuffer psCB : register(b1)
{
    float4 ambient;
    float3 lightPosition;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 worldPosition : POSITION0;
    float3 worldNormal : NORMAL;
    float4 color : COLOR0;
};

//
// Pixel shader entrypoint
//
float4 main(PS_INPUT i) : SV_TARGET
{
    // Calculate diffusion amount
    float3 lightDir = normalize(lightPosition);
    float diff = saturate(dot(i.worldNormal, -lightDir));
    
    // Calculate output color
    float3 col = ambient.rgb + (i.color.rgb * diff);
    
    return float4(col, 1.0f);
}