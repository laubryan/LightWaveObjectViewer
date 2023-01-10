#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <assert.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "RendererDefinitions.h"
#include "LightWaveObject/LightWaveObject.h"

class Renderer {
public:

	// Public methods
	bool Initialize(HWND outputWindow, UINT width, UINT height);
	bool LoadObject(std::string objectPathname);
	void Present();
	void Render();
	void Shutdown();
	void Update();

private:

	// Private member functdions
	bool InitializeDirect3D();
	bool InitializeDevice();
	bool InitializeRenderTargetView();
	bool InitializeShaders();
	bool InitializeDepthStencil();
	bool InitializeViewport();

	bool InitializeBuffers();
	bool InitializeObjectTransforms();
	bool InitializeLights();

	ID3DBlob* CompileShaderFromFile(LPCWSTR shaderPathname, LPCSTR compilerTarget);

	bool TransferMeshDataFromLWO(unique_ptr<LightWaveObject> obj);

	// Private data
	
	// D3D
	ID3D11Device* _device;					// D3D device
	ID3D11DeviceContext* _deviceContext;	// D3D device context
	IDXGISwapChain* _swapChain;				// D3D swap chain
	ID3D11RenderTargetView* _renderTargetView;	// Render target view

	// Depth stencil
	ID3D11DepthStencilView* _depthStencilView;
	ID3D11DepthStencilState* _depthStencilState;

	// Shaders
	ID3D11VertexShader* _vertexShader;
	ID3D11PixelShader* _pixelShader;
	ID3D11InputLayout* _inputLayout;

	// Buffers
	ID3D11Buffer* _vertexBuffer;			// Object vertex buffer
	ID3D11Buffer* _indexBuffer;				// Object index buffer

	// Vertex shader constant buffer
	ID3D11Buffer* _vsConstantBuffer;					
	CONSTANT_BUFFER_VS _vsConstantBufferData;

	// Pixel shader constant buffer
	ID3D11Buffer* _psConstantBuffer;					
	CONSTANT_BUFFER_PS _psConstantBufferData;

	// Window
	D3D11_VIEWPORT _viewport;
	D3D_FEATURE_LEVEL _deviceFeatureLevel;	// Created device feature level
	HWND _outputWindow;						// Output window handle
	UINT _windowWidth;
	UINT _windowHeight;

	// Mesh
	std::vector<VERTEX> _vertices;
	std::vector<WORD> _indices;

	// Transformations
	DirectX::XMMATRIX _objectTranslation;
	DirectX::XMMATRIX _viewMatrix;
	DirectX::XMMATRIX _projectionMatrix;

	// Timer
	ULONGLONG _time = 0;
};

