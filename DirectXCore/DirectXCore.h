#ifndef DIRECTXCORE_H
#define DIRECTXCORE_H

#include <d3dx12.h>
#include <dxgi1_6.h>

// DirectXのコア部分
class DirectXCore
{
public:

	// コンストラクタ
	DirectXCore();

	// デストラクタ
	~DirectXCore();

	// 初期化
	bool Initialize(HWND hWnd, int windowWidth, int windowHeight);

	// 解放
	void Release();

	// デバイスを取得
	ID3D12Device* GetDevice() { return pDevice; }

	// 毎フレームの処理
	void Tick();

private:
	ID3D12Device* pDevice;
	IDXGIFactory6* pDxgiFactory;
	ID3D12CommandAllocator* pCommandAllocator;
	ID3D12GraphicsCommandList* pCommandList;
	ID3D12CommandQueue* pCommandQueue;
	IDXGISwapChain4* pSwapChain;
	std::vector<ID3D12Resource*> buffers;
	ID3D12DescriptorHeap* pDescriptorHeap;
	ID3D12Fence* pFence;
	UINT64 fenceValue;

	// 初期化
	bool init(HWND hWnd, int windowWidth, int windowHeight);

	// 描画
	void Render();
};

#endif		// #ifndef DIRECTXCORE_H
