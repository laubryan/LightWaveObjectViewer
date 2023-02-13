//
// Vertex Shader
//

cbuffer ModelViewProjectionCB : register(b0)
{
    matrix world;
    matrix worldView;
    matrix worldViewProj;
};

struct VS_INPUT
{
    float4 pos : POSITION;
    float4 normal : NORMAL0;
    float4 col : COLOR0;
};

struct VS_OUTPUT
{
    float4 pos : SV_Position;
    float3 worldPosition : POSITION0;
    float3 worldNormal : NORMAL0;
    float4 col : COLOR0;
};

//
// Vertex shader entrypoint
//
VS_OUTPUT main(VS_INPUT i)
{
    VS_OUTPUT o;
    
    // Pass through some values
    o.col = i.col;
    
    // Transform normal
    o.worldNormal = normalize(mul(i.normal, world)).xyz;
    
    // Calculate vertex world position
    o.worldPosition = mul(i.pos, world).xyz;
    
    // Apply view-projection transformation
    o.pos = mul(i.pos, worldViewProj);
    
    return o;
}
