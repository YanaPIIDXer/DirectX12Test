#ifndef DIRECTXCORE_H
#define DIRECTXCORE_H

#include <d3dx12.h>
#include <dxgi1_6.h>

// DirectX�̃R�A����
class DirectXCore
{
public:

	// �R���X�g���N�^
	DirectXCore();

	// �f�X�g���N�^
	~DirectXCore();

	// ������
	bool Initialize(HWND hWnd, int windowWidth, int windowHeight);

	// ���
	void Release();

	// ���t���[���̏���
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

	// ������
	bool init(HWND hWnd, int windowWidth, int windowHeight);

	// �`��
	void Render();
};

#endif		// #ifndef DIRECTXCORE_H
