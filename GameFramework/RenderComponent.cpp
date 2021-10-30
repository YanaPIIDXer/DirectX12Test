#include "RenderComponent.h"
#include "Scene.h"

// コンストラクタ
RenderComponent::RenderComponent(Actor *pInOwner, Scene* pInScene)
	: Component(pInOwner)
	, pScene(pInScene)
{
	pScene->AddRenderComponent(this);
}

// デストラクタ
RenderComponent::~RenderComponent()
{
	pScene->RemoveRenderComponent(this);
}
