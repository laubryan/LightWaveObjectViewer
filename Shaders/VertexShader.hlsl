//
// Vertex Shader
//

cbuffer ModelViewProjectionCB : register(b0)
{
    matrix worldViewProj;
};

struct VS_INPUT
{
    float4 pos : POSITION;
    float3 normal : NORMAL0;
    float3 col : COLOR0;
};

struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float3 col : COLOR0;
};

//
// Vertex shader entrypoint
//
VS_OUTPUT main(VS_INPUT i)
{
    VS_OUTPUT o;
    
    // Pass through some values
    o.col = i.col;
    
    // Apply view-projection transformation
    o.pos = mul(i.pos, worldViewProj);
    
    return o;
}
