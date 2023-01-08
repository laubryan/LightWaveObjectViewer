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
// Per-frame constant buffer
//
struct CONSTANT_BUFFER_FRAME {
	DirectX::XMFLOAT4X4 model;
};

//
// Static constant buffer
//
struct CONSTANT_BUFFER_STATIC {
	DirectX::XMFLOAT4X4 lightPosition;
	DirectX::XMFLOAT4X4 viewProjection;
};

//
// Shader constants
//
const LPCSTR SHADER_ENTRY_POINT = "main";
const LPCSTR VS_COMPILER_TARGET = "vs_4_0";
const LPCSTR PS_COMPILER_TARGET = "ps_4_0";
