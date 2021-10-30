#include "TextureComponent.h"
#include <Windows.h>
#include <tchar.h>
#include <d3dx12.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>

// 安全にRELEASE_SAFEease関数を実行するためのマクロ
#define RELEASE_SAFE(p) if (p != nullptr) { p->Release(); p = nullptr; }

// メッセージボックス
#define MSGBOX(text, caption) MessageBox(nullptr, _T(text), _T(caption), MB_OK)

// コンストラクタ
TextureComponent::TextureComponent(Actor* pInOwner, Scene* pInScene)
	: RenderComponent(pInOwner, pInScene)
	, pVertexBuffer(nullptr)
	, vertexBufferView({})
	, pIndexBuffer(nullptr)
	, indexBufferView({})
	, pVertexShader(nullptr)
	, pPixelShader(nullptr)
	, pRootSignature(nullptr)
	, pPipelineState(nullptr)
	, pTextureBuffer(nullptr)
	, pTextureDescHeap(nullptr)
{
}

// デストラクタ
TextureComponent::~TextureComponent()
{
	RELEASE_SAFE(pTextureDescHeap);
	RELEASE_SAFE(pTextureBuffer);
	RELEASE_SAFE(pPipelineState);
	RELEASE_SAFE(pRootSignature);
	RELEASE_SAFE(pPixelShader);
	RELEASE_SAFE(pVertexShader);
	RELEASE_SAFE(pIndexBuffer);
	RELEASE_SAFE(pVertexBuffer);
}

// テクスチャ読み込み
bool TextureComponent::LoadTexture(ID3D12Device* pDevice, const wchar_t* pFilePath)
{
	{
		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		const Vertex vertices[] = {
			{ { -0.4f, -0.7f, 0.0f }, { 0.0f, 1.0f } },
			{ { -0.4f, 0.7f, 0.0f }, { 0.0f, 0.0f } },
			{ { 0.4f, -0.7f, 0.0f }, { 1.0f, 1.0f } },
			{ { 0.4f, 0.7f, 0.0f  }, { 1.0f, 0.0f } }
		};

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

		const unsigned short indices[] = {
			0, 1, 2, 2, 1, 3
		};

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

	{
		TexMetadata metadata = {};
		ScratchImage scratchImg = {};
		if (FAILED(LoadFromWICFile(pFilePath, WIC_FLAGS_NONE, &metadata, scratchImg)))
		{
			MSGBOX("テクスチャの読み込みに失敗しました", "Error");
			return false;
		}

		auto pImg = scratchImg.GetImage(0, 0, 0);

		D3D12_HEAP_PROPERTIES heapProp = {};
		heapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
		heapProp.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
		heapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
		heapProp.CreationNodeMask = 0;
		heapProp.VisibleNodeMask = 0;

		D3D12_RESOURCE_DESC desc = {};
		desc.Format = metadata.format;
		desc.Width = metadata.width;
		desc.Height = metadata.height;
		desc.DepthOrArraySize = (UINT16)metadata.arraySize;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.MipLevels = (UINT16)metadata.mipLevels;
		desc.Dimension = static_cast<D3D12_RESOURCE_DIMENSION>(metadata.dimension);
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (FAILED(pDevice->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, nullptr, IID_PPV_ARGS(&pTextureBuffer))) ||
			FAILED(pTextureBuffer->WriteToSubresource(0, nullptr, pImg->pixels, pImg->rowPitch, pImg->slicePitch)))
		{
			MSGBOX("TextureBufferの生成に失敗しました", "Error");
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
			MSGBOX("TextureDescHeapの生成に失敗しました", "Error");
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

// 描画
void TextureComponent::Render(ID3D12GraphicsCommandList* pCommandList)
{
	pCommandList->SetPipelineState(pPipelineState);
	pCommandList->SetGraphicsRootSignature(pRootSignature);
	pCommandList->SetDescriptorHeaps(1, &pTextureDescHeap);
	pCommandList->SetGraphicsRootDescriptorTable(0, pTextureDescHeap->GetGPUDescriptorHandleForHeapStart());
	pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pCommandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	pCommandList->IASetIndexBuffer(&indexBufferView);
	pCommandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
}
