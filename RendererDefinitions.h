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
	DirectX::XMFLOAT4X4 world;
	DirectX::XMFLOAT4X4 worldView;
	DirectX::XMFLOAT4X4 worldViewProj;
};

//
// Pixel shader constant buffer
//
struct CONSTANT_BUFFER_PS {
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT3 lightPosition;
	bool padding;
};

//
// Shader constants
//
const LPCSTR SHADER_ENTRY_POINT = "main";
const LPCSTR VS_COMPILER_TARGET = "vs_4_0";
const LPCSTR PS_COMPILER_TARGET = "ps_4_0";
