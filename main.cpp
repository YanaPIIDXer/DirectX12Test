#include <Windows.h>
#include <vector>
#include <tchar.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

using namespace DirectX;

// ���S��RELEASE_SAFEease�֐������s���邽�߂̃}�N��
#define RELEASE_SAFE(p) if (p != nullptr) { p->Release(); p = nullptr; }

#define MSGBOX(text, caption) MessageBox(nullptr, _T(text), _T(caption), MB_OK)

ID3D12Device* pDevice = nullptr;
IDXGIFactory6* pDxgiFactory = nullptr;
ID3D12CommandAllocator* pCommandAllocator = nullptr;
ID3D12GraphicsCommandList* pCommandList = nullptr;
ID3D12CommandQueue* pCommandQueue = nullptr;
IDXGISwapChain4* pSwapChain = nullptr;
ID3D12DescriptorHeap* pDescriptorHeap = nullptr;
ID3D12Fence* pFence = nullptr;
UINT64 fenceValue = 0;

const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

const XMFLOAT3 vertices[] = {
	{ -1.0f, -1.0f, 0.0f },
	{ -1.0f, 1.0f, 0.0f },
	{ 1.0f, -1.0f, 0.0f }
};
ID3D12Resource* pVertexBuffer = nullptr;

// DirectX�̏�����
bool InitD3DX(HWND hWnd)
{
	D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };

	for (auto level : featureLevels)
	{
		if (SUCCEEDED(D3D12CreateDevice(nullptr, level, IID_PPV_ARGS(&pDevice))))
		{
			break;
		}
	}
	if (pDevice == nullptr)
	{
		MSGBOX("Device�̐����Ɏ��s���܂���", "Error");
		return false;
	}

	if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&pDxgiFactory))))
	{
		MSGBOX("DXGIFactory�̐����Ɏ��s���܂���", "Error");
		return false;
	}

	if (FAILED(pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&pCommandAllocator))))
	{
		MSGBOX("CommandAllocator�̐����Ɏ��s���܂���", "Error");
		return false;
	}

	if (FAILED(pDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, pCommandAllocator, nullptr, IID_PPV_ARGS(&pCommandList))))
	{
		MSGBOX("CommandList�̐����Ɏ��s���܂���", "Error");
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
			MSGBOX("CommandQueue�̐����Ɏ��s���܂���", "Error");
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
			MSGBOX("SwapChain�̐����Ɏ��s���܂���", "Error");
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
			MSGBOX("DescriptorHeap�̐����Ɏ��s���܂���", "Error");
			return false;
		}
	}

	{
		DXGI_SWAP_CHAIN_DESC desc = {};
		pSwapChain->GetDesc(&desc);
		std::vector<ID3D12Resource*> buffers(desc.BufferCount);
		auto handle = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < desc.BufferCount; i++)
		{
			pSwapChain->GetBuffer(i, IID_PPV_ARGS(&buffers[i]));
			handle.ptr += i * pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			pDevice->CreateRenderTargetView(buffers[i], nullptr, handle);
		}
	}

	if (FAILED(pDevice->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&pFence))))
	{
		MSGBOX("Fence�̏������Ɏ��s���܂���", "Error");
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
			MSGBOX("VertexBuffer�̐����Ɏ��s���܂���", "Error");
			return false;
		}
	}

	return true;
}

// �`��
void Render()
{
	// ��InitD3DX���Ă΂��O��WM_PAINT�����s�����\��������̂ŁA
	//   pDevice�����������Ă��邩�ǂ����Ŕ��肷��
	if (pDevice == nullptr) { return; }

	pCommandAllocator->Reset();
	
	auto bufferIndex = pSwapChain->GetCurrentBackBufferIndex();
	D3D12_RESOURCE_BARRIER barrierDesc = {};
	barrierDesc.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrierDesc.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	ID3D12Resource* pBuffer = nullptr;
	pSwapChain->GetBuffer(bufferIndex, IID_PPV_ARGS(&pBuffer));
	barrierDesc.Transition.pResource = pBuffer;
	barrierDesc.Transition.Subresource = 0;
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	pCommandList->ResourceBarrier(1, &barrierDesc);

	auto handle = pDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += bufferIndex * pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	pCommandList->OMSetRenderTargets(1, &handle, true, nullptr);

	float backgroundColor[] = { 0.0f, 0.0f, 1.0f, 1.0f };
	pCommandList->ClearRenderTargetView(handle, backgroundColor, 0, nullptr);

	pCommandList->Close();

	ID3D12CommandList* commandLists[] = { pCommandList };
	pCommandQueue->ExecuteCommandLists(1, commandLists);

	pCommandQueue->Signal(pFence, fenceValue);
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

	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	pCommandList->ResourceBarrier(1, &barrierDesc);
}

// DirectX�̉��
void RELEASE_SAFEeaseD3DX()
{
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
		MSGBOX("DirectX�̏������Ɏ��s���܂���", "Error");
		// ���C�����[�v�ɓ���Ȃ��悤�ɂ��āA������������S�ɍs����悤��
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

	RELEASE_SAFEeaseD3DX();

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
