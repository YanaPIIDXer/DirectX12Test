#include "DirectXCore.h"
#include <tchar.h>

// 安全にRELEASE_SAFEease関数を実行するためのマクロ
#define RELEASE_SAFE(p) if (p != nullptr) { p->Release(); p = nullptr; }

// メッセージボックス
#define MSGBOX(text, caption) MessageBox(nullptr, _T(text), _T(caption), MB_OK)

// コンストラクタ
DirectXCore::DirectXCore()
	: pDevice(nullptr)
	, pDxgiFactory(nullptr)
	, pCommandAllocator(nullptr)
	, pCommandList(nullptr)
	, pCommandQueue(nullptr)
	, pSwapChain(nullptr)
	, pDescriptorHeap(nullptr)
	, pFence(nullptr)
	, fenceValue(0)
{
}

// デストラクタ
DirectXCore::~DirectXCore()
{
	Release();
}

// 初期化
bool DirectXCore::Initialize(HWND hWnd, int windowWidth, int windowHeight)
{
	if (!init(hWnd, windowWidth, windowHeight))
	{
		Release();
		return false;
	}
	return true;
}

// 解放
void DirectXCore::Release()
{
	RELEASE_SAFE(pFence);
	RELEASE_SAFE(pDescriptorHeap);
	for (auto pBuffer : buffers)
	{
		RELEASE_SAFE(pBuffer);
	}
	RELEASE_SAFE(pSwapChain);
	RELEASE_SAFE(pCommandQueue);
	RELEASE_SAFE(pCommandList);
	RELEASE_SAFE(pCommandAllocator);
	RELEASE_SAFE(pDxgiFactory);
	RELEASE_SAFE(pDevice);
}

// 毎フレームの処理
void DirectXCore::Tick()
{
	Render();
}


// 初期化
bool DirectXCore::init(HWND hWnd, int windowWidth, int windowHeight)
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
		desc.Width = windowWidth;
		desc.Height = windowHeight;
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

	return true;
}

// 描画
void DirectXCore::Render()
{
	auto bufferIndex = pSwapChain->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrierDesc = {};
	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrierDesc.Transition.pResource = buffers[bufferIndex];
	barrierDesc.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	pCommandList->ResourceBarrier(1, &barrierDesc);

	auto handle = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += bufferIndex * pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	pCommandList->OMSetRenderTargets(1, &handle, false, nullptr);

	float backgroundColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	pCommandList->ClearRenderTargetView(handle, backgroundColor, 0, nullptr);

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
