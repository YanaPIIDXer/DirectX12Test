#ifndef RENDERABLE_H
#define RENDERABLE_H

class ID3D12GraphicsCommandList;

// 描画可能Componentに実装するインタフェース
class IRenderable
{
public:
	// 描画
	virtual void Render(ID3D12GraphicsCommandList* pCommandList) = 0;

	virtual ~IRenderable() = 0;
};

#endif		// #ifndef RENDERABLE_H
