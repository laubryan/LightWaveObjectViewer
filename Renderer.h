#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <assert.h>
#include <stdio.h>
#include <vector>

#include "RendererDefinitions.h"

class Renderer {
public:

	// Public methods
	bool Initialize(HWND outputWindow, UINT width, UINT height);
	bool LoadTestObject();
	bool LoadTestObject2();
	void Present();
	void Render();
	void Shutdown();
	void Update();

private:

	// Private member functdions
	bool InitializeBuffers();
	bool InitializeDirect3D();
	bool InitializeDevice();
	bool InitializeRenderTargetView();
	bool InitializeShaders();
	bool InitializeDepthStencil();
	bool InitializeView();
	bool InitializeViewport();

	ID3DBlob* CompileShaderFromFile(LPCWSTR shaderPathname, LPCSTR compilerTarget);

	// Private data
	
	// D3D
	ID3D11Device* device_;					// D3D device
	ID3D11DeviceContext* deviceContext_;	// D3D device context
	IDXGISwapChain* swapChain_;				// D3D swap chain
	ID3D11RenderTargetView* renderTargetView_;	// Render target view

	// Depth stencil
	ID3D11DepthStencilView* depthStencilView_;
	ID3D11DepthStencilState* depthStencilState_;

	// Shaders
	ID3D11VertexShader* vertexShader_;
	ID3D11PixelShader* pixelShader_;
	ID3D11InputLayout* inputLayout_;

	// Buffers
	ID3D11Buffer* vertexBuffer_;			// Object vertex buffer
	ID3D11Buffer* indexBuffer_;				// Object index buffer

	// Constant buffer
	ID3D11Buffer* constantBuffer_;			// Shader constant buffer

	// Constant buffer data definition
	struct VS_CONSTANT_BUFFER_DATA {
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	} constantBufferData_;

	// Window
	D3D11_VIEWPORT viewport_;
	D3D_FEATURE_LEVEL deviceFeatureLevel_;	// Created device feature level
	HWND outputWindow_;						// Output window handle
	UINT windowWidth_;
	UINT windowHeight_;

	// Mesh
	std::vector<VERTEX> vertices_;
	std::vector<WORD> indices_;
};

