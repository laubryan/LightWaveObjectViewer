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
    DirectX::XMFLOAT4 color;
};

//
// Shader constants
//
const LPCSTR SHADER_ENTRY_POINT = "main";
const LPCSTR VS_COMPILER_TARGET = "vs_4_0";
const LPCSTR PS_COMPILER_TARGET = "ps_4_0";
