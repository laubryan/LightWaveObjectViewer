#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <assert.h>
#include <stdio.h>
#include <string>
#include <vector>

#include "ObjectReader.h"
#include "RendererDefinitions.h"

class Renderer {
public:

	// Structures
	struct ObjectInfo {
		int numLayers = 0;
		int numNonTriangles = 0;
		int numTriangles = 0;
		int numVertices = 0;
	};

	// Getters
	ObjectInfo	GetObjectInfo();

	// Public methods
	void AdjustViewDistance(int direction);
	bool Initialize(HWND outputWindow, UINT width, UINT height);
	bool LoadObject(std::string objectPathname);
	void Present();
	void Render();
	void ResetTransformations();
	void Rotate(float yaw, float pitch);
	void Shutdown();
	void Tumble(bool tumble);
	void Update();

private:

	// Private member functions
	bool InitializeDepthBuffer();
	bool InitializeDirect3D();
	bool InitializeDevice();
	bool InitializeRenderTargetView();
	bool InitializeShaders();
	bool InitializeViewport();

	bool InitializeBuffers();
	bool InitializeObjectTransforms();
	bool InitializeLights();

	ID3DBlob* CompileShaderFromFile(LPCWSTR shaderPathname, LPCSTR compilerTarget);
	float GetObjectWidth();

	// Private data
	
	// D3D
	ID3D11Device* _device;					// D3D device
	ID3D11DeviceContext* _deviceContext;	// D3D device context
	IDXGISwapChain* _swapChain;				// D3D swap chain
	ID3D11RenderTargetView* _renderTargetView;	// Render target view

	// Depth stencil
	ID3D11DepthStencilView* _depthStencilView {};
	ID3D11DepthStencilState* _depthStencilState {};

	// Shaders
	ID3D11VertexShader* _vertexShader {};
	ID3D11PixelShader* _pixelShader {};
	ID3D11InputLayout* _inputLayout {};

	// Buffers
	ID3D11Buffer* _vertexBuffer {};			// Object vertex buffer
	ID3D11Buffer* _indexBuffer {};			// Object index buffer

	// Vertex shader constant buffer
	ID3D11Buffer* _vsConstantBuffer {};					
	CONSTANT_BUFFER_VS _vsConstantBufferData {};

	// Pixel shader constant buffer
	ID3D11Buffer* _psConstantBuffer {};					
	CONSTANT_BUFFER_PS _psConstantBufferData {};

	// Window
	D3D11_VIEWPORT _viewport;
	D3D_FEATURE_LEVEL _deviceFeatureLevel;	// Created device feature level
	HWND _outputWindow;						// Output window handle
	UINT _windowWidth;
	UINT _windowHeight;

	// Mesh
	std::vector<VERTEX> _vertices;
	std::vector<WORD> _indices;

	// Object info
	ObjectInfo _objectInfo;

	// Transformations
	DirectX::XMMATRIX _modelMatrix;
	DirectX::XMMATRIX _viewMatrix;
	DirectX::XMMATRIX _projectionMatrix;
	float _viewZ;

	// State
	bool _tumble {true};

	// Timer
	ULONGLONG _time = 0;
};

