//
// Game.cpp
//

#include "pch.h"
#include "Game.h"

extern void ExitGame() noexcept;

using namespace DirectX;

using Microsoft::WRL::ComPtr;

struct Vertex
{
	XMFLOAT4 position;
	XMFLOAT2 texture;
	XMFLOAT3 normal;
};

struct LightBuffer
{
	XMFLOAT4 diffuseColor;
	XMFLOAT3 lightDirection;
	float padding;	// Add extra padding to make the buffer size a multiple of 16.
};

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    // TODO: Change the timer settings if you want something other than the default variable timestep mode.
    // e.g. for 60 FPS fixed timestep update logic, call:
    /*
    m_timer.SetFixedTimeStep(true);
    m_timer.SetTargetElapsedSeconds(1.0 / 60);
    */
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float elapsedTime = float(timer.GetElapsedSeconds());

    // TODO: Add your game logic here.
    elapsedTime;
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

    // TODO: Add your rendering code here.
	UINT strides = sizeof(Vertex);
	UINT offsets = 0;

	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &strides, &offsets);
	context->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(m_inputLayout.Get());

	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);
	context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());
	context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());
	context->PSSetConstantBuffers(0, 1, m_lightBuffer.GetAddressOf());

	context->DrawIndexed(3, 0, 0);

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::Black);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto device = m_deviceResources->GetD3DDevice();

    // TODO: Initialize device dependent objects here (independent of window size).
	DX::ThrowIfFailed(
		CreateWICTextureFromFile(device, L"D:/Dev/Direct3D Demos/Textures/light_old.jpg",
			nullptr, m_texture.ReleaseAndGetAddressOf()));

	// Setup vertex buffer.
	Vertex vertices[3]
	{
		{XMFLOAT4(-1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)},
		{XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(0.5f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)},
		{XMFLOAT4(1.0f, -1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f)}
	};

	D3D11_BUFFER_DESC vertexBufferDesc{};
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 3;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA vertexData{};
	vertexData.pSysMem = vertices;

	DX::ThrowIfFailed(device->CreateBuffer(&vertexBufferDesc, &vertexData, m_vertexBuffer.GetAddressOf()));

	// Setup index buffer.
	UINT indices[3]
	{
		0, 1, 2
	};

	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(UINT) * 3;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA indexData{};
	indexData.pSysMem = indices;

	DX::ThrowIfFailed(device->CreateBuffer(&indexBufferDesc, &indexData, m_indexBuffer.GetAddressOf()));

	// Setup constant buffer.
	LightBuffer lightBuffer
	{
		XMFLOAT4(0.94509804f, 0.72156862f, 0.17647058f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f),
		0.0f
	};

	D3D11_BUFFER_DESC lightBufferDesc{};
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.ByteWidth = sizeof(LightBuffer);
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA lightBufferData{};
	lightBufferData.pSysMem = &lightBuffer;

	DX::ThrowIfFailed(device->CreateBuffer(&lightBufferDesc, &lightBufferData, m_lightBuffer.GetAddressOf()));

	// Load shaders.
	auto vertexShaderBlob = DX::ReadData(L"D:/Dev/Direct3D Demos/Debug/DiffuseVS.cso");

	DX::ThrowIfFailed(
		device->CreateVertexShader(vertexShaderBlob.data(), vertexShaderBlob.size(), nullptr,
			m_vertexShader.GetAddressOf()));

	auto pixelShaderBlob = DX::ReadData(L"D:/Dev/Direct3D Demos/Debug/DiffusePS.cso");

	DX::ThrowIfFailed(
		device->CreatePixelShader(pixelShaderBlob.data(), pixelShaderBlob.size(), nullptr,
			m_pixelShader.GetAddressOf()));

	// Create input layout.
	D3D11_INPUT_ELEMENT_DESC inputElements[3]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	DX::ThrowIfFailed(
		device->CreateInputLayout(inputElements, 3, vertexShaderBlob.data(), vertexShaderBlob.size(),
			m_inputLayout.GetAddressOf()));

	// Create texture sampler state.
	D3D11_SAMPLER_DESC samplerDesc{};
	samplerDesc.Filter = D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	DX::ThrowIfFailed(device->CreateSamplerState(&samplerDesc, m_samplerState.GetAddressOf()));
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    // TODO: Initialize windows-size dependent objects here.
}

void Game::CreateTexture()
{
	
}

void Game::OnDeviceLost()
{
    // TODO: Add Direct3D resource cleanup here.
	m_texture.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion
