#include <Windows.h>
#include <vector>
#include <tchar.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <string>
#include <time.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

// à¿ëSÇ…RELEASE_SAFEeaseä÷êîÇé¿çsÇ∑ÇÈÇΩÇﬂÇÃÉ}ÉNÉç
#define RELEASE_SAFE(p) if (p != nullptr) { p->Release(); p = nullptr; }

#define MSGBOX(text, caption) MessageBox(nullptr, _T(text), _T(caption), MB_OK)

ID3D12Device* pDevice = nullptr;
IDXGIFactory6* pDxgiFactory = nullptr;
ID3D12CommandAllocator* pCommandAllocator = nullptr;
ID3D12GraphicsCommandList* pCommandList = nullptr;
ID3D12CommandQueue* pCommandQueue = nullptr;
IDXGISwapChain4* pSwapChain = nullptr;
std::vector<ID3D12Resource*> buffers;
ID3D12DescriptorHeap* pDescriptorHeap = nullptr;
ID3D12Fence* pFence = nullptr;
UINT64 fenceValue = 0;
D3D12_VIEWPORT viewport = {};
D3D12_RECT scissorRect = {};

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 texCoord;
};

const Vertex vertices[] = {
	{ { -0.4f, -0.7f, 0.0f }, { 0.0f, 1.0f } },
	{ { -0.4f, 0.7f, 0.0f }, { 0.0f, 0.0f } },
	{ { 0.4f, -0.7f, 0.0f }, { 1.0f, 1.0f } },
	{ { 0.4f, 0.7f, 0.0f  }, { 1.0f, 0.0f } }
};
ID3D12Resource* pVertexBuffer = nullptr;
D3D12_VERTEX_BUFFER_VIEW vertexBufferView = {};

const unsigned short indices[] = {
	0, 1, 2, 2, 1, 3
};
ID3D12Resource* pIndexBuffer = nullptr;
D3D12_INDEX_BUFFER_VIEW indexBufferView = {};

ID3DBlob* pVertexShader = nullptr;
ID3DBlob* pPixelShader = nullptr;
ID3D12RootSignature* pRootSignature = nullptr;
ID3D12PipelineState* pPipelineState = nullptr;

struct PixelColor
{
	unsigned char R, G, B, A;
};
std::vector<PixelColor> texData(256 * 256);
ID3D12Resource* pTextureBuffer = nullptr;
ID3D12DescriptorHeap* pTextureDescHeap = nullptr;

// DirectXÇÃèâä˙âª
bool InitD3DX(HWND hWnd)
{
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&pDxgiFactory))))
	{
		MSGBOX("DXGIFactoryÇÃê∂ê¨Ç…é∏îsÇµÇ‹ÇµÇΩ", "Error");
		return false;
	}

	std::vector <IDXGIAdapter*> adapters;
	IDXGIAdapter* pUseAdapter = nullptr;
	for (int i = 0; pDxgiFactory->EnumAdapters(i, &pUseAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		adapters.push_back(pUseAdapter);
	}
	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC desc = {};
		adpt->GetDesc(&desc);
		std::wstring strDesc = desc.Description;
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			pUseAdapter = adpt;
			break;
		}
	}

	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
	for (auto level : featureLevels)
	{
		if (SUCCEEDED(D3D12CreateDevice(pUseAdapter, level, IID_PPV_ARGS(&pDevice))))
		{
			break;
		}
	}
	if (pDevice == nullptr)
	{
		MSGBOX("DeviceÇÃê∂ê¨Ç…é∏îsÇµÇ‹ÇµÇΩ", "Error");
		return false;
	}

	if (FAILED(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocator))))
	{
		MSGBOX("CommandAllocatorÇÃê∂ê¨Ç…é∏îsÇµÇ‹ÇµÇΩ", "Error");
		return false;
	}

	if (FAILED(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator, nullptr, IID_PPV_ARGS(&pCommandList))))
	{
		MSGBOX("CommandListÇÃê∂ê¨Ç…é∏îsÇµÇ‹ÇµÇΩ", "Error");
		return false;
	}

	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.NodeMask = 0;
		desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		if (FAILED(pDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&pCommandQueue))))
		{
			MSGBOX("CommandQueueÇÃê∂ê¨Ç…é∏îsÇµÇ‹ÇµÇΩ", "Error");
			return false;
		}
	}

	{
		DXGI_SWAP_CHAIN_DESC1 desc = {};
		desc.Width = WINDOW_WIDTH;
		desc.Height = WINDOW_HEIGHT;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Stereo = false;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
		desc.BufferCount = 2;
		desc.Scaling = DXGI_SCALING_STRETCH;
		desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		desc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		if (FAILED(pDxgiFactory->CreateSwapChainForHwnd(pCommandQueue, hWnd, &desc, nullptr, nullptr, (IDXGISwapChain1**)&pSwapChain)))
		{
			MSGBOX("SwapChainÇÃê∂ê¨Ç…é∏îsÇµÇ‹ÇµÇΩ", "Error");
			return false;
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		desc.NodeMask = 0;
		desc.NumDescriptors = 2;
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
		if (FAILED(pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pDescriptorHeap))))
		{
			MSGBOX("DescriptorHeapÇÃê∂ê¨Ç…é∏îsÇµÇ‹ÇµÇΩ", "Error");
			return false;
		}
	}

	{
		DXGI_SWAP_CHAIN_DESC desc = {};
		pSwapChain->GetDesc(&desc);

		buffers.resize(desc.BufferCount);
		auto handle = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < desc.BufferCount; i++)
		{
			pSwapChain->GetBuffer(i, IID_PPV_ARGS(&buffers[i]));
			pDevice->CreateRenderTargetView(buffers[i], nullptr, handle);
			handle.ptr += pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}
	}

	if (FAILED(pDevice->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence))))
	{
		MSGBOX("FenceÇÃèâä˙âªÇ…é∏îsÇµÇ‹ÇµÇΩ", "Error");
		return false;
	}
	
	{
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC desc = {};
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = sizeof(vertices);
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		if (FAILED(pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pVertexBuffer))))
		{
			MSGBOX("VertexBufferÇÃê∂ê¨Ç…é∏îsÇµÇ‹ÇµÇΩ", "Error");
			return false;
		}

		Vertex* pVertexMap = nullptr;
		pVertexBuffer->Map(0, nullptr, (void**)&pVertexMap);
		std::copy(std::begin(vertices), std::end(vertices), pVertexMap);
		pVertexBuffer->Unmap(0, nullptr);

		vertexBufferView.BufferLocation = pVertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.SizeInBytes = sizeof(vertices);
		vertexBufferView.StrideInBytes = sizeof(vertices[0]);

		desc.Width = sizeof(indices);
		if (FAILED(pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&pIndexBuffer))))
		{
			MSGBOX("IndexBufferÇÃê∂ê¨Ç…é∏îsÇµÇ‹ÇµÇΩ", "Error");
			return false;
		}

		unsigned short* pIndexMap = nullptr;
		pIndexBuffer->Map(0, nullptr, (void**)&pIndexMap);
		std::copy(std::begin(indices), std::end(indices), pIndexMap);
		pIndexBuffer->Unmap(0, nullptr);

		indexBufferView.BufferLocation = pIndexBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = DXGI_FORMAT_R16_UINT;
		indexBufferView.SizeInBytes = sizeof(indices);
	}

	if (FAILED(D3DCompileFromFile(L"VertexShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "BasicVS", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pVertexShader, nullptr)))
	{
		MSGBOX("VertexShaderÇÃÉRÉìÉpÉCÉãÇ…é∏îsÇµÇ‹ÇµÇΩ", "Error");
		return false;
	}

	if (FAILED(D3DCompileFromFile(L"PixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "BasicPS", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pPixelShader, nullptr)))
	{
		MSGBOX("PixelShaderÇÃÉRÉìÉpÉCÉãÇ…é∏îsÇµÇ‹ÇµÇΩ", "Error");
		return false;
	}

	{
		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		{
			D3D12_DESCRIPTOR_RANGE range = {};
			range.NumDescriptors = 1;
			range.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
			range.BaseShaderRegister = 0;
			range.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

			D3D12_ROOT_PARAMETER rootParam = {};
			rootParam.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
			rootParam.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			rootParam.DescriptorTable.pDescriptorRanges = &range;
			rootParam.DescriptorTable.NumDescriptorRanges = 1;

			D3D12_STATIC_SAMPLER_DESC samplerDesc = {};
			samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
			samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
			samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
			samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
			samplerDesc.MinLOD = 0.0f;
			samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
			samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;

			D3D12_ROOT_SIGNATURE_DESC desc = {};
			desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			desc.pParameters = &rootParam;
			desc.NumParameters = 1;
			desc.pStaticSamplers = &samplerDesc;
			desc.NumStaticSamplers = 1;

			ID3DBlob* pBlob = nullptr;
			if (FAILED(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pBlob, nullptr)) ||
				FAILED(pDevice->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(&pRootSignature))))
			{
				MSGBOX("RootSignatureÇÃê∂ê¨Ç…é∏îsÇµÇ‹ÇµÇΩÅB", "Error");
				return false;
			}
			RELEASE_SAFE(pBlob);
		}

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature = pRootSignature;
		desc.VS.pShaderBytecode = pVertexShader->GetBufferPointer();
		desc.VS.BytecodeLength = pVertexShader->GetBufferSize();
		desc.PS.pShaderBytecode = pPixelShader->GetBufferPointer();
		desc.PS.BytecodeLength = pPixelShader->GetBufferSize();
		desc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
		desc.RasterizerState.MultisampleEnable = false;
		desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		desc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
		desc.RasterizerState.DepthClipEnable = true;
		desc.BlendState.AlphaToCoverageEnable = false;
		desc.BlendState.IndependentBlendEnable = false;

		D3D12_RENDER_TARGET_BLEND_DESC blendDesc = {};
		blendDesc.BlendEnable = false;
		blendDesc.LogicOpEnable = false;
		blendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

		desc.BlendState.RenderTarget[0] = blendDesc;
		desc.InputLayout.pInputElementDescs = inputLayout;
		desc.InputLayout.NumElements = _countof(inputLayout);

		desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;

		if (FAILED(pDevice->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&pPipelineState))))
		{
			MSGBOX("PipelineStateÇÃê∂ê¨Ç…é∏îsÇµÇ‹ÇµÇΩ", "Error");
			return false;
		}
	}

	viewport.Width = WINDOW_WIDTH;
	viewport.Height = WINDOW_HEIGHT;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;

	scissorRect.top = 0;
	scissorRect.left = 0;
	scissorRect.right = scissorRect.left + WINDOW_WIDTH;
	scissorRect.bottom = scissorRect.top + WINDOW_HEIGHT;

	{
		for (auto& color : texData)
		{
			color.R = rand() % 256;
			color.G = rand() % 256;
			color.B = rand() % 256;
			color.A = 255;
		}

		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC desc = {};
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Width = 256;
		desc.Height = 256;
		desc.DepthOrArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.MipLevels = 1;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (FAILED(pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(&pTextureBuffer))) ||
			FAILED(pTextureBuffer->WriteToSubresource(0, nullptr, texData.data(), sizeof(PixelColor) * 256, sizeof(PixelColor) * texData.size())))
		{
			MSGBOX("TextureBufferÇÃê∂ê¨Ç…é∏îsÇµÇ‹ÇµÇΩ", "Error");
			return false;
		}
	}

	{
		D3D12_DESCRIPTOR_HEAP_DESC desc = {};
		desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		desc.NodeMask = 0;
		desc.NumDescriptors = 1;
		desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		if (FAILED(pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&pTextureDescHeap))))
		{
			MSGBOX("TextureDescHeapÇÃê∂ê¨Ç…é∏îsÇµÇ‹ÇµÇΩ", "Error");
			return false;
		}
	}

	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MipLevels = 1;

		pDevice->CreateShaderResourceView(pTextureBuffer, &desc, pTextureDescHeap->GetCPUDescriptorHandleForHeapStart());
	}
	return true;
}

// ï`âÊ
void Render()
{
	// Å´InitD3DXÇ™åƒÇŒÇÍÇÈëOÇ…WM_PAINTÇ™î≠çsÇ≥ÇÍÇÈâ¬î\ê´Ç™Ç†ÇÈÇÃÇ≈ÅA
	//   pDeviceÇèâä˙âªÇµÇƒÇ¢ÇÈÇ©Ç«Ç§Ç©Ç≈îªíËÇ∑ÇÈ
	if (pDevice == nullptr) { return; }

	auto bufferIndex = pSwapChain->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrierDesc = {};
	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrierDesc.Transition.pResource = buffers[bufferIndex];
	barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	pCommandList->ResourceBarrier(1, &barrierDesc);

	pCommandList->SetPipelineState(pPipelineState);

	auto handle = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += bufferIndex * pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	pCommandList->OMSetRenderTargets(1, &handle, false, nullptr);

	float backgroundColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	pCommandList->ClearRenderTargetView(handle, backgroundColor, 0, nullptr);

	pCommandList->RSSetViewports(1, &viewport);
	pCommandList->RSSetScissorRects(1, &scissorRect);
	pCommandList->SetGraphicsRootSignature(pRootSignature);
	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	pCommandList->IASetIndexBuffer(&indexBufferView);
	pCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);

	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	pCommandList->ResourceBarrier(1, &barrierDesc);

	pCommandList->Close();

	ID3D12CommandList* commandLists[] = { pCommandList };
	pCommandQueue->ExecuteCommandLists(1, commandLists);

	pCommandQueue->Signal(pFence, ++fenceValue);
	if (pFence->GetCompletedValue() != fenceValue)
	{
		auto event = CreateEvent(nullptr, false, false, nullptr);
		if (event != nullptr)
		{
			pFence->SetEventOnCompletion(fenceValue, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
	}

	pCommandAllocator->Reset();
	pCommandList->Reset(pCommandAllocator, nullptr);

	pSwapChain->Present(1, 0);
}

// DirectXÇÃâï˙
void ReleaseD3DX()
{
	RELEASE_SAFE(pTextureDescHeap);
	RELEASE_SAFE(pTextureBuffer);
	RELEASE_SAFE(pPipelineState);
	RELEASE_SAFE(pRootSignature);
	RELEASE_SAFE(pPixelShader);
	RELEASE_SAFE(pVertexShader);
	RELEASE_SAFE(pIndexBuffer);
	RELEASE_SAFE(pVertexBuffer);
	RELEASE_SAFE(pFence);
	RELEASE_SAFE(pDescriptorHeap);
	RELEASE_SAFE(pSwapChain);
	RELEASE_SAFE(pCommandQueue);
	RELEASE_SAFE(pCommandList);
	RELEASE_SAFE(pCommandAllocator);
	RELEASE_SAFE(pDxgiFactory);
	RELEASE_SAFE(pDevice);
}

LRESULT WindowProc(HWND, UINT, WPARAM, LPARAM);

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_  HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	srand((unsigned int)time(nullptr));
	const LPCWSTR className = L"DirectX12Test";

	WNDCLASSEX w = {};
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;
	w.lpszClassName = className;
	w.hInstance = GetModuleHandle(nullptr);
	RegisterClassEx(&w);

	RECT wrc = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	HWND hWnd = CreateWindow(className, className, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, wrc.right - wrc.left, wrc.bottom - wrc.top, nullptr, nullptr, w.hInstance, nullptr);
	ShowWindow(hWnd, SW_SHOW);

	MSG msg = {};

	if (!InitD3DX(hWnd))
	{
		MSGBOX("DirectXÇÃèâä˙âªÇ…é∏îsÇµÇ‹ÇµÇΩ", "Error");
		// ÉÅÉCÉìÉãÅ[ÉvÇ…ì¸ÇÁÇ»Ç¢ÇÊÇ§Ç…ÇµÇƒÅAâï˙èàóùÇ™à¿ëSÇ…çsÇÌÇÍÇÈÇÊÇ§Ç…
		msg.message = WM_QUIT;
	}

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
		}
	}

	ReleaseD3DX();

	UnregisterClass(className, w.hInstance);
	return 0;
}

LRESULT WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_DESTROY:

			PostQuitMessage(0);
			break;

		case WM_PAINT:
			Render();
			break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
