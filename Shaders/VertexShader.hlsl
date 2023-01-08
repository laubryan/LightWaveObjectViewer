//
// Vertex Shader
//

cbuffer ModelViewProjectionCB : register(b0)
{
    matrix model;
};

cbuffer LightCB : register(b1)
{
    matrix lightPosition;
    matrix viewProjection;
};

struct VS_INPUT
{
    float3 pos : POSITION;
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
    
    // Fetch vertex position
    float4 pos = float4(i.pos, 1.0f);
    
    // Transform vertex to world space
    pos = mul(pos, model);
    
    // Transform camera to world
    //pos = mul(pos, view);
    
    // Apply projection matrix
    //o.pos = mul(pos, projection);
    
    // Apply view-projection transformation
    o.pos = mul(pos, viewProjection);
    
    return o;
}
