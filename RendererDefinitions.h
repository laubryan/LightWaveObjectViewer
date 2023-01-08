//
// Renderer Definitions
//
#pragma once
#include <DirectXMath.h>

//
// Vertex structure
//
struct VERTEX {
    DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT4 color;
};

//
// Vertex shader constant buffer
//
struct CONSTANT_BUFFER_VS {
	DirectX::XMFLOAT4X4 worldViewProj;
};

//
// Pixel shader constant buffer
//
struct CONSTANT_BUFFER_PS {
	DirectX::XMFLOAT4X4 lightPosition;
	DirectX::XMFLOAT4X4 viewProjection;
};

//
// Shader constants
//
const LPCSTR SHADER_ENTRY_POINT = "main";
const LPCSTR VS_COMPILER_TARGET = "vs_4_0";
const LPCSTR PS_COMPILER_TARGET = "ps_4_0";
