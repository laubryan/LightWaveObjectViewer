#include "Renderer.h"

/// <summary>
/// Initialize renderer
/// </summary>
bool Renderer::Initialize(HWND outputWindow, UINT width, UINT height) {

	// Save parameters
	_outputWindow = outputWindow;
	_windowWidth = width;
	_windowHeight = height;

	// Direct3D
	if (!InitializeDirect3D()) return false;

	// Success
	return true;
}

/// <summary>
/// Load object
/// </summary>
/// <returns>Load success state</returns>
bool Renderer::LoadObject() {

	// Load test object
	LoadTestObject();

	// Buffers
	if (!InitializeBuffers()) return false;

	return true;
}

/// <summary>
/// Present the current frame
/// </summary>
void Renderer::Present() {

	// Flip the back buffer
	HRESULT hr = _swapChain->Present(0, 0);
	assert(!FAILED(hr));
}

/// <summary>
/// Render a frame
/// </summary>
void Renderer::Render() {

	// Update constant buffer
	_deviceContext->UpdateSubresource(_constantBuffer, 0, nullptr, &_constantBufferData, 0, 0);

	// Bind render target (Output-Merger stage)
	_deviceContext->OMSetRenderTargets(1, &_renderTargetView, nullptr);

	// Clear render target
	float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	_deviceContext->ClearRenderTargetView(_renderTargetView, clearColor);

	// Set shader constant buffers
	_deviceContext->VSSetConstantBuffers(0, 1, &_constantBuffer);

	// Set vertex shader stage
	_deviceContext->VSSetShader(_vertexShader, nullptr, 0);

	// Set pixel shader stage
	_deviceContext->PSSetShader(_pixelShader, nullptr, 0);

	// Draw indexed triangles
	_deviceContext->DrawIndexed(_indices.size(), 0, 0);
}

/// <summary>
/// Shut down renderer
/// </summary>
void Renderer::Shutdown() {

	// Depth stencil
	if (_depthStencilState) _depthStencilState->Release();
	if (_depthStencilView) _depthStencilView->Release();

	// Release shader resources
	_vertexShader->Release();
	_pixelShader->Release();
	_inputLayout->Release();
	_constantBuffer->Release();

	// Release object buffers
	_vertexBuffer->Release();
	_indexBuffer->Release();

	// D3D resources
	_renderTargetView->Release();
	_swapChain->Release();
	_deviceContext->Release();
	_device->Release();
}

/// <summary>
/// Update the scene
/// </summary>
void Renderer::Update() {

	// Get current time
	ULONGLONG currentTime = GetTickCount64();

	// Initialize start time
	if (_time == 0) _time = currentTime;

	// Calculate elapsed time
	float elapsedTime = (float) (currentTime - _time);

	// Calculate object rotation
	float rotation = elapsedTime / 1000.0f;
	_objectRotation = DirectX::XMMatrixRotationRollPitchYaw(rotation, rotation, 0);

	// Update model matrix
	DirectX::XMStoreFloat4x4(&_constantBufferData.model, DirectX::XMMatrixMultiply(_objectTranslation, _objectRotation));
}

/// <summary>
/// Initialize vertex, pixel and constant buffers
/// </summary>
/// <returns>Initialized status</returns>
bool Renderer::InitializeBuffers() {

	// Describe vertex buffer description and initialization data
	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
	bufferDescription.Usage = D3D11_USAGE_DEFAULT;
	bufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDescription.ByteWidth = sizeof(VERTEX) * _vertices.size();

	// Create vertex buffer
	D3D11_SUBRESOURCE_DATA vertexInitData;
	ZeroMemory(&vertexInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexInitData.pSysMem = &_vertices[0];
	HRESULT hr = _device->CreateBuffer(&bufferDescription, &vertexInitData, &_vertexBuffer);
	if (FAILED(hr)) return false;

	// Set vertex buffer (Input-Assembler stage)
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	// Configure index buffer description
	bufferDescription.Usage = D3D11_USAGE_DEFAULT;
	bufferDescription.ByteWidth = sizeof(WORD) * _indices.size();
	bufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDescription.CPUAccessFlags = 0;

	// Configure index buffer initialization data
	D3D11_SUBRESOURCE_DATA indexInitData;
	ZeroMemory(&indexInitData, sizeof(D3D11_SUBRESOURCE_DATA));
	indexInitData.pSysMem = &_indices[0];

	// Create index buffer
	hr = _device->CreateBuffer(&bufferDescription, &indexInitData, &_indexBuffer);
	if (FAILED(hr)) return false;

	// Set index buffer
	_deviceContext->IASetIndexBuffer(_indexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// Configure primitive topology
	_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

/// <summary>
/// Initialize Direct3D device and context
/// </summary>
/// <returns>Initialized status</returns>
bool Renderer::InitializeDirect3D() {

	// Device
	if (!InitializeDevice()) return false;

	// Render target view
	if (!InitializeRenderTargetView()) return false;

	// Viewport
	if (!InitializeViewport()) return false;

	// Shaders
	if (!InitializeShaders()) return false;

	// Depth stencil
	//if (!InitializeDepthStencil()) return false;

	// View
	InitializeView();

	// Success
	return true;
}

/// <summary>
/// Initialize D3D device
/// </summary>
/// <returns>Initialization state</returns>
bool Renderer::InitializeDevice() {

	// Set D3D supported feature levels for this app
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_11_1
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	// Set device support flags
	UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Configure swap chain parameters
	DXGI_SWAP_CHAIN_DESC swapChainParams;
	ZeroMemory(&swapChainParams, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainParams.Windowed = TRUE;
	swapChainParams.BufferCount = 2;
	swapChainParams.BufferDesc.Width = _windowWidth;
	swapChainParams.BufferDesc.Height = _windowHeight;
	swapChainParams.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainParams.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainParams.SampleDesc.Count = 1;
	swapChainParams.SampleDesc.Quality = 0;
	swapChainParams.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
	swapChainParams.OutputWindow = _outputWindow;

	// Create D3D device, context and swap chain
	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,                    // Use the default adapter
		D3D_DRIVER_TYPE_HARDWARE,   // Use the hardware driver
		0,                          // Not using software
		deviceFlags,                // Debug and Direct2D compatibility flags
		featureLevels,              // Supported feature levels
		numFeatureLevels,           // Size of the feature levels array
		D3D11_SDK_VERSION,          // D3D SDK version
		&swapChainParams,           // Swap chain parameters
		&_swapChain,                // Swap chain
		&_device,                   // Return created D3D device
		&_deviceFeatureLevel,       // Return selected feature level
		&_deviceContext             // Return created D3D device context
	);

	// Check for errors
	if (FAILED(hr)) {
		return false;
	}

	// Success
	return true;
}

/// <summary>
/// Initialize render target view
/// </summary>
/// <returns>Initialized status</returns>
bool Renderer::InitializeRenderTargetView() {

	// Get back buffer from swap chain
	ID3D11Resource* backBuffer;
	HRESULT hr = _swapChain->GetBuffer(0, __uuidof(backBuffer), (void**)&backBuffer);
	assert(!FAILED(hr));

	// Create render target view
	hr = _device->CreateRenderTargetView(backBuffer, NULL, &_renderTargetView);
	assert(!FAILED(hr));

	// Release the ref count on the back buffer (from GetBuffer)
	backBuffer->Release();

	return true;
}

/// <summary>
/// Initialize vertex and pixel shaders
/// </summary>
/// <returns>Initialized status</returns>
bool Renderer::InitializeShaders() {

	HRESULT hr;

	// Set shader parameters
	LPCWSTR vsFilename = L"Shaders\\VertexShader.hlsl";
	LPCWSTR psFilename = L"Shaders\\PixelShader.hlsl";

	///////////////////////////////////////
	// Vertex Shader

	// Compile vertex shader from file
	ID3DBlob* compiledVS = CompileShaderFromFile(vsFilename, VS_COMPILER_TARGET);
	if (compiledVS == nullptr) {
		MessageBox(nullptr, L"Error compiling vertex shader", L"Shader Initialization Error", MB_OK);
		return false;
	}

	// Create shader from compiled code
	hr = _device->CreateVertexShader(compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), nullptr, &_vertexShader);
	if (FAILED(hr)) {
		if (compiledVS) compiledVS->Release();
		MessageBox(nullptr, L"Error creating vertex shader", L"Shader Initialization Error", MB_OK);
		return false;
	}

	///////////////////////////////////////
	// Pixel Shader

	// Compile pixel shader from file
	ID3DBlob* compiledPS = CompileShaderFromFile(psFilename, PS_COMPILER_TARGET);
	if (compiledPS == nullptr) {
		MessageBox(nullptr, L"Error compiling pixel shader", L"Shader Initialization Error", MB_OK);
		return false;
	}

	// Create shader from compiled code
	hr = _device->CreatePixelShader(compiledPS->GetBufferPointer(), compiledPS->GetBufferSize(), nullptr, &_pixelShader);
	if (FAILED(hr)) {
		if (compiledVS) compiledVS->Release();
		if (compiledPS) compiledPS->Release();
		MessageBox(nullptr, L"Error creating pixel shader", L"Shader Initialization Error", MB_OK);
		return false;
	}

	///////////////////////////////////////
	// Input Layout

	// Define shader input layout
	D3D11_INPUT_ELEMENT_DESC layoutDescription[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	// { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	UINT numLayoutElements = ARRAYSIZE(layoutDescription);

	// Create input layout
	hr = _device->CreateInputLayout(layoutDescription, numLayoutElements, compiledVS->GetBufferPointer(), compiledVS->GetBufferSize(), &_inputLayout);
	if (FAILED(hr)) {
		if (compiledVS) compiledVS->Release();
		if (compiledPS) compiledPS->Release();
		return false;
	}

	// Set input layout for shaders
	_deviceContext->IASetInputLayout(_inputLayout);

	// Release resources
	if (compiledVS) compiledVS->Release();
	if (compiledPS) compiledPS->Release();

	return true;
}

/// <summary>
/// Initialize depth stencil
/// </summary>
/// <returns>Initialized status</returns>
bool Renderer::InitializeDepthStencil() {

	// Describe depth stencil
	D3D11_TEXTURE2D_DESC depthStencilTextureDescription;
	ZeroMemory(&depthStencilTextureDescription, sizeof(D3D11_TEXTURE2D_DESC));
	depthStencilTextureDescription.Width = _windowWidth;
	depthStencilTextureDescription.Height = _windowHeight;
	depthStencilTextureDescription.MipLevels = 1;
	depthStencilTextureDescription.ArraySize = 1;
	depthStencilTextureDescription.SampleDesc.Count = 1;
	depthStencilTextureDescription.SampleDesc.Quality = 0;
	depthStencilTextureDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilTextureDescription.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// Create depth stencil texture
	ID3D11Texture2D* depthStencilTexture;
	HRESULT hr = _device->CreateTexture2D(&depthStencilTextureDescription, nullptr, &depthStencilTexture);
	assert(!FAILED(hr));

	// Set depth stencil state
	D3D11_DEPTH_STENCIL_DESC depthStencilDescription;
	ZeroMemory(&depthStencilDescription, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthStencilDescription.DepthEnable = TRUE;
	depthStencilDescription.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDescription.DepthFunc = D3D11_COMPARISON_LESS;
	hr = _device->CreateDepthStencilState(&depthStencilDescription, &_depthStencilState);

	// Bind the depth stencil state to the OM stage
	_deviceContext->OMSetDepthStencilState(_depthStencilState, 1);

	// Create the depth stencil
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDescription;
	ZeroMemory(&depthStencilViewDescription, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	depthStencilViewDescription.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDescription.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDescription.Texture2D.MipSlice = 0;
	hr = _device->CreateDepthStencilView(depthStencilTexture, &depthStencilViewDescription, &_depthStencilView);
	assert(!FAILED(hr));

	return true;
}

/// <summary>
/// Initialize view and perspective matrices
/// </summary>
/// <returns>Initialized status</returns>
bool Renderer::InitializeView() {

	// Set up model matrix
	DirectX::XMMATRIX modelMatrixTranspose = DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f));
	DirectX::XMStoreFloat4x4(&_constantBufferData.model, modelMatrixTranspose);

	// Set up view matrix
	DirectX::XMVECTOR eyePt = DirectX::XMVectorSet(0.0f, 0.0f, -5.0f, 0.0f);
	DirectX::XMVECTOR lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR upVec = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX lookAtMatrix = DirectX::XMMatrixLookAtRH(eyePt, lookAt, upVec);
	DirectX::XMMATRIX lookAtMatrixTranspose = DirectX::XMMatrixTranspose(lookAtMatrix);
	DirectX::XMStoreFloat4x4(&_constantBufferData.view, lookAtMatrixTranspose);

	// Set up projection matrix
	float aspectRatio = ((float)_windowWidth / (float)_windowHeight);
	float fovAngleY = 70.0f * DirectX::XM_PI / 180.0f;
	float nearPlane = 0.01f;
	float farPlane = 500.0f;
	DirectX::XMMATRIX perspectiveMatrix = DirectX::XMMatrixPerspectiveFovRH(fovAngleY, aspectRatio, nearPlane, farPlane);
	DirectX::XMMATRIX perspectiveMatrixTranspose = DirectX::XMMatrixTranspose(perspectiveMatrix);
	DirectX::XMStoreFloat4x4(&_constantBufferData.projection, perspectiveMatrixTranspose);

	// Define constant buffer elements to feed transformations to shader
	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(_constantBufferData), D3D11_BIND_CONSTANT_BUFFER);

	// Create constant buffer 
	HRESULT hr = _device->CreateBuffer(&constantBufferDesc, nullptr, &_constantBuffer);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

/// <summary>
/// Initialize viewport
/// </summary>
/// <returns>Initialized status</returns>
bool Renderer::InitializeViewport() {

	// Initialize viewport parameters
	ZeroMemory(&_viewport, sizeof(D3D11_VIEWPORT));
	_viewport.Height = (float)_windowWidth;
	_viewport.Width = (float)_windowHeight;
	_viewport.MinDepth = 0;
	_viewport.MaxDepth = 1;

	// Set viewport
	_deviceContext->RSSetViewports(1, &_viewport);

	return true;
}

/// <summary>
/// Compile shader from source file
/// </summary>
/// <param name="shaderPathname">Pathname of HLSL source file</param>
/// <param name="compilerTarget">Compiler target string</param>
/// <returns>Compiled shader</returns>
ID3DBlob* Renderer::CompileShaderFromFile(LPCWSTR shaderPathname, LPCSTR compilerTarget) {

	ID3DBlob* compiledShader = nullptr;
	ID3DBlob* compilationErrors;

	// Set compile flags
	// Set compilation flags
#if _DEBUG
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	// Compile shader from file
	HRESULT hr = D3DCompileFromFile(shaderPathname, nullptr, nullptr, SHADER_ENTRY_POINT, compilerTarget, compileFlags, 0, &compiledShader, &compilationErrors);
	if (FAILED(hr)) {

		// Compilation error
		OutputDebugString((LPCWSTR)compilationErrors->GetBufferPointer());

		// Release resources
		if (compiledShader) compiledShader->Release();
	}

	// Release resources
	if (compilationErrors) compilationErrors->Release();

	return compiledShader;
}

/// <summary>
/// Load test object
/// </summary>
bool Renderer::LoadTestObject() {

	// Define object vertices in list
	_vertices = {
		{ DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(-0.58f, -0.58f, -0.58f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, // 0
		{ DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT3(-0.58f, -0.58f,  0.58f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, // 1
		{ DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT3(-0.58f,  0.58f, -0.58f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }, // 2
		{ DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3(-0.58f,  0.58f,  0.58f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, // 3
		{ DirectX::XMFLOAT3( 0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3( 0.58f, -0.58f, -0.58f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, // 4
		{ DirectX::XMFLOAT3( 0.5f, -0.5f,  0.5f), DirectX::XMFLOAT3( 0.58f, -0.58f,  0.58f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) }, // 5
		{ DirectX::XMFLOAT3( 0.5f,  0.5f, -0.5f), DirectX::XMFLOAT3( 0.58f,  0.58f, -0.58f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }, // 6
		{ DirectX::XMFLOAT3( 0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3( 0.58f,  0.58f,  0.58f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) }, // 7
	};

	// Define indices
	_indices = {
		0, 2, 1,
		1, 2, 3,
		4, 5, 6,
		5, 7, 6,
		0, 1, 5,
		0, 5, 4,
		2, 6, 7,
		2, 7, 3,
		0, 4, 6,
		0, 6, 2,
		1, 3, 7,
		1, 7, 5,
	};

	// Set initial transformations
	_objectTranslation = DirectX::XMMatrixTranslation(0, 0, 0);
	_objectRotation = DirectX::XMMatrixIdentity();

	// Success
	return true;
}
