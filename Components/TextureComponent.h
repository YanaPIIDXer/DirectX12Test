#ifndef TEXTURECOMPONENT_H
#define TEXTURECOMPONENT_H

#include <d3d12.h>
#include <DirectXMath.h>
#include "../GameFramework/RenderComponent.h"

using namespace DirectX;

// 頂点情報
struct Vertex
{
	XMFLOAT3 position;
	XMFLOAT2 texCoord;
};

// テクスチャコンポーネント
class TextureComponent : public RenderComponent
{
public:
	// コンストラクタ
	TextureComponent(Actor* pInOwner, Scene* pInScene);

	// デストラクタ
	virtual ~TextureComponent();

	// テクスチャ読み込み
	bool LoadTexture(ID3D12Device* pDevice, const wchar_t *pFilePath);
	
	// 更新
	// 何もしない
	virtual void Update() override {}

	// 描画
	virtual void Render(ID3D12GraphicsCommandList* pCommandList) override;

private:
	ID3D12Resource* pVertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	ID3D12Resource* pIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	ID3DBlob* pVertexShader;
	ID3DBlob* pPixelShader;
	ID3D12RootSignature* pRootSignature;
	ID3D12PipelineState* pPipelineState;

	ID3D12Resource* pTextureBuffer;
	ID3D12DescriptorHeap* pTextureDescHeap;
};

#endif		// #ifndef TEXTURECOMPONENT_H
