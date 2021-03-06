#ifndef DIRECTXCORE_H
#define DIRECTXCORE_H

#include <d3dx12.h>
#include <dxgi1_6.h>
#include <memory>

class Scene;

// DirectXのコア部分
class DirectXCore
{
public:

	// コンストラクタ
	DirectXCore();

	// デストラクタ
	~DirectXCore();

	// 初期化
	bool Initialize(HWND hWnd, int windowWidth, int windowHeight, Scene* pInitialScene);

	// 解放
	void Release();

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
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
	std::shared_ptr<Scene> pCurrentScene;

	// 初期化
	bool init(HWND hWnd, int windowWidth, int windowHeight, Scene* pInitialScene);

	// 描画
	void Render();
};

#endif		// #ifndef DIRECTXCORE_H
