#include <Windows.h>
#include <vector>
#include <tchar.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <string>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

// 安全にRELEASE_SAFEease関数を実行するためのマクロ
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

// DirectXの初期化
bool InitD3DX(HWND hWnd)
{
	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&pDxgiFactory))))
	{
		MSGBOX("DXGIFactoryの生成に失敗しました", "Error");
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
		MSGBOX("Deviceの生成に失敗しました", "Error");
		return false;
	}

	if (FAILED(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocator))))
	{
		MSGBOX("CommandAllocatorの生成に失敗しました", "Error");
		return false;
	}

	if (FAILED(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator, nullptr, IID_PPV_ARGS(&pCommandList))))
	{
		MSGBOX("CommandListの生成に失敗しました", "Error");
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
			MSGBOX("CommandQueueの生成に失敗しました", "Error");
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
			MSGBOX("SwapChainの生成に失敗しました", "Error");
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
			MSGBOX("DescriptorHeapの生成に失敗しました", "Error");
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
		MSGBOX("Fenceの初期化に失敗しました", "Error");
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
			MSGBOX("VertexBufferの生成に失敗しました", "Error");
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
			MSGBOX("IndexBufferの生成に失敗しました", "Error");
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
		MSGBOX("VertexShaderのコンパイルに失敗しました", "Error");
		return false;
	}

	if (FAILED(D3DCompileFromFile(L"PixelShader.hlsl", nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, "BasicPS", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pPixelShader, nullptr)))
	{
		MSGBOX("PixelShaderのコンパイルに失敗しました", "Error");
		return false;
	}

	{
		D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		};

		{
			D3D12_ROOT_SIGNATURE_DESC desc = {};
			desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
			ID3DBlob* pBlob = nullptr;
			if (FAILED(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, &pBlob, nullptr)) ||
				FAILED(pDevice->CreateRootSignature(0, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), IID_PPV_ARGS(&pRootSignature))))
			{
				MSGBOX("RootSignatureの生成に失敗しました。", "Error");
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
			MSGBOX("PipelineStateの生成に失敗しました", "Error");
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

	return true;
}

// 描画
void Render()
{
	// ↓InitD3DXが呼ばれる前にWM_PAINTが発行される可能性があるので、
	//   pDeviceを初期化しているかどうかで判定する
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

// DirectXの解放
void ReleaseD3DX()
{
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
		MSGBOX("DirectXの初期化に失敗しました", "Error");
		// メインループに入らないようにして、解放処理が安全に行われるように
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
