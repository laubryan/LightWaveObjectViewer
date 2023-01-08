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
// Constant buffer definition
// Constant buffer data definition
struct VS_CONSTANT_BUFFER_DATA {
	DirectX::XMFLOAT4X4 model;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

//
// Shader constants
//
const LPCSTR SHADER_ENTRY_POINT = "main";
const LPCSTR VS_COMPILER_TARGET = "vs_4_0";
const LPCSTR PS_COMPILER_TARGET = "ps_4_0";
