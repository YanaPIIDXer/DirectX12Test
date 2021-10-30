#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include "Component.h"

class Scene;
class ID3D12GraphicsCommandList;

// 描画Component
class RenderComponent : public Component
{
public:

	// コンストラクタ
	RenderComponent(Actor *pInOwner, Scene* pInScene);

	// デストラクタ
	virtual ~RenderComponent() = 0;

	// 描画
	virtual void Render(ID3D12GraphicsCommandList* pCommandList) = 0;

private:

	// 所有シーン
	Scene* pScene;
};

#endif		// #ifndef RENDERCOMPONENT_H
