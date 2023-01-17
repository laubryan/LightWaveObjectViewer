#include "Renderer.h"

/// <summary>
/// Adjust view distance in specified direction
/// </summary>
/// <param name="direction">Adjustment direction</param>
void Renderer::AdjustViewDistance(int direction) {

	float step = 0.1f;

	if (direction > 0) {
		_viewZ += step;
	}
	else {
		_viewZ -= step;
	}
}

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

	// Lights
	if (!InitializeLights()) return false;

	// Success
	return true;
}

/// <summary>
/// Load object
/// </summary>
/// <returns>Load success state</returns>
bool Renderer::LoadObject(std::string objectPathname) {

	// Extract mesh data from object
	ObjectReader reader;
	string errorReason;
	if (!reader.ReadObjectFile(objectPathname, errorReason)) {
		MessageBoxA(_outputWindow, errorReason.c_str(), "Error Loading Object", MB_OK);
		return false;
	}

	// Free old buffers if required
	if(_vertexBuffer) _vertexBuffer->Release();
	if(_indexBuffer) _indexBuffer->Release();
	if(_vsConstantBuffer) _vsConstantBuffer->Release();
	if(_psConstantBuffer) _psConstantBuffer->Release();

	// Get object vertices and indices
	_vertices = reader.GetVertices();
	_indices = reader.GetIndices();

	// Buffers
	if (!InitializeBuffers()) return false;

	// Initialize transforms
	if (!InitializeObjectTransforms()) return false;

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

	// Update constant buffers
	_deviceContext->UpdateSubresource(_vsConstantBuffer, 0, nullptr, &_vsConstantBufferData, 0, 0);
	_deviceContext->UpdateSubresource(_psConstantBuffer, 0, nullptr, &_psConstantBufferData, 0, 0);

	// Bind render target (Output-Merger stage)
	_deviceContext->OMSetRenderTargets(1, &_renderTargetView, _depthStencilView);

	// Clear render target
	float clearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
	_deviceContext->ClearRenderTargetView(_renderTargetView, clearColor);
	_deviceContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set constant buffers
	_deviceContext->VSSetConstantBuffers(0, 1, &_vsConstantBuffer); // Register b0
	_deviceContext->PSSetConstantBuffers(1, 1, &_psConstantBuffer); // Register b1

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

	// Shader resources
	_vertexShader->Release();
	_pixelShader->Release();
	_inputLayout->Release();

	// Constant buffers
	if(_vsConstantBuffer) _vsConstantBuffer->Release();
	if(_psConstantBuffer) _psConstantBuffer->Release();

	// Object buffers
	if(_vertexBuffer) _vertexBuffer->Release();
	if(_indexBuffer) _indexBuffer->Release();

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
	float elapsedTime = (float)(currentTime - _time);

	// Calculate object rotation
	float rotation = elapsedTime / 2000.0f;
	DirectX::XMMATRIX objectRotation = DirectX::XMMatrixRotationRollPitchYaw(rotation, rotation, 0);
	DirectX::XMMATRIX worldMatrix = _objectTranslation * objectRotation;
	DirectX::XMStoreFloat4x4(&_vsConstantBufferData.world, worldMatrix);

	// Calculate view matrix
	DirectX::XMVECTOR eyePt = DirectX::XMVectorSet(0.0f, 0.0f, _viewZ, 0.0f);
	DirectX::XMVECTOR lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMVECTOR upVec = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	_viewMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtRH(eyePt, lookAt, upVec));

	// Calculate projection matrix
	float aspectRatio = (float)_windowWidth / (float)_windowHeight;
	float fovAngleY = 45.0f * (DirectX::XM_PI / 180.0f);
	float nearPlane = 0.01f;
	float farPlane = 500.0f;
	_projectionMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovRH(fovAngleY, aspectRatio, nearPlane, farPlane));

	// Update world-view
	DirectX::XMMATRIX worldViewMatrix = worldMatrix * _viewMatrix;
	DirectX::XMStoreFloat4x4(&_vsConstantBufferData.worldView, worldViewMatrix);

	// Update world-view-projection
	DirectX::XMMATRIX worldViewProjectionMatrix = _projectionMatrix * _viewMatrix * worldMatrix;
	DirectX::XMStoreFloat4x4(&_vsConstantBufferData.worldViewProj, worldViewProjectionMatrix);
}

/// <summary>
/// Initialize the depth buffer
/// </summary>
/// <returns>Initialization success</returns>
bool Renderer::InitializeDepthBuffer() {

	HRESULT hr;

	// Initialize buffer description
	D3D11_TEXTURE2D_DESC depthTextureDesc;
	ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
	depthTextureDesc.Width = _windowWidth;
	depthTextureDesc.Height = _windowHeight;
	depthTextureDesc.MipLevels = 1;
	depthTextureDesc.ArraySize = 1;
	depthTextureDesc.SampleDesc.Count = 1;
	depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	// Create the depth stencil texture
	ID3D11Texture2D* depthStencilTexture;
	hr = _device->CreateTexture2D(&depthTextureDesc, NULL, &depthStencilTexture);

	// Check for errors
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Error creating depth stencil texture", L"Depth Buffer Initialization Error", MB_OK);
		return false;
	}

	// Initialize depth stencil view description
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = depthTextureDesc.Format;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

	// Create the depth stencil view
	hr = _device->CreateDepthStencilView(depthStencilTexture, &depthStencilViewDesc, &_depthStencilView);
	depthStencilTexture->Release();

	// Check for errors
	if (FAILED(hr)) {
		MessageBox(nullptr, L"Error creating depth stencil view", L"Depth Buffer Initialization Error", MB_OK);
		return false;
	}

	return true;
}

/// <summary>
/// Initialize Direct3D device and context
/// </summary>
/// <returns>Initialization success</returns>
bool Renderer::InitializeDirect3D() {

	// Device
	if (!InitializeDevice()) return false;

	// Depth buffer
	if (!InitializeDepthBuffer()) return false;

	// Render target view
	if (!InitializeRenderTargetView()) return false;

	// Viewport
	if (!InitializeViewport()) return false;

	// Shaders
	if (!InitializeShaders()) return false;

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
/// <returns>Initialization success</returns>
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
/// <returns>Initialization success</returns>
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
/// Initialize viewport
/// </summary>
/// <returns>Initialization success</returns>
bool Renderer::InitializeViewport() {

	// Initialize viewport parameters
	ZeroMemory(&_viewport, sizeof(D3D11_VIEWPORT));
	_viewport.Width = (float)_windowWidth;
	_viewport.Height = (float)_windowHeight;
	_viewport.MinDepth = 0;
	_viewport.MaxDepth = 1;

	// Set viewport
	_deviceContext->RSSetViewports(1, &_viewport);

	return true;
}

/// <summary>
/// Initialize vertex, pixel and constant buffers
/// </summary>
/// <returns>Initialization success</returns>
bool Renderer::InitializeBuffers() {

	HRESULT hr;

	///////////////////////////////////////
	// Vertex Buffer

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
	hr = _device->CreateBuffer(&bufferDescription, &vertexInitData, &_vertexBuffer);
	if (FAILED(hr)) return false;

	// Set vertex buffer (Input-Assembler stage)
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	_deviceContext->IASetVertexBuffers(0, 1, &_vertexBuffer, &stride, &offset);

	///////////////////////////////////////
	// Index Buffer

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

	///////////////////////////////////////
	// Constant Buffers

	// Create vertex shader constant buffer 
	CD3D11_BUFFER_DESC vsConstantBufferDesc(sizeof(_vsConstantBufferData), D3D11_BIND_CONSTANT_BUFFER);
	hr = _device->CreateBuffer(&vsConstantBufferDesc, nullptr, &_vsConstantBuffer);
	if (FAILED(hr)) {
		return false;
	}

	// Create pixel shader constant buffer 
	CD3D11_BUFFER_DESC psConstantBufferDesc(sizeof(_psConstantBufferData), D3D11_BIND_CONSTANT_BUFFER);
	hr = _device->CreateBuffer(&psConstantBufferDesc, nullptr, &_psConstantBuffer);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

/// <summary>
/// Initialize and store object transformations
/// </summary>
/// <returns></returns>
bool Renderer::InitializeObjectTransforms() {

	// Initialize object translation
	_objectTranslation = DirectX::XMMatrixTranslation(0.0f, 0.0f, 0.0f);

	// Get object dimensions
	float objectWidth = GetObjectWidth();

	// Calculate required view distance
	_viewZ = (objectWidth / 2.0f) / tan(45.0f) * -15.0f;

	return true;
}

/// <summary>
/// Initialize lights
/// </summary>
/// <returns></returns>
bool Renderer::InitializeLights() {

	// Store ambient light
	_psConstantBufferData.ambient = { 0.2f, 0.2f, 0.2f, 1.0f };

	// Store light position
	_psConstantBufferData.lightPosition = DirectX::XMFLOAT3(10.0f, 0.0f, 10.0f);

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
/// Get width of object from vertex data
/// </summary>
/// <returns>Object width in units</returns>
float Renderer::GetObjectWidth() {

	// Initialize min and max dimensions
	float minX = _vertices[0].pos.x;
	float maxX = minX;

	// Check all vertices
	for (VERTEX& vertex : _vertices) {

		// Update min and max
		if (vertex.pos.x < minX) {
			minX = vertex.pos.x;
		}
		else if (vertex.pos.x > maxX) {
			maxX = vertex.pos.x;
		}
	}

	return maxX - minX;
}

