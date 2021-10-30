#include "RenderComponent.h"
#include "Scene.h"

// コンストラクタ
RenderComponent::RenderComponent(Actor *pInOwner, Scene* pInScene)
	: Component(pInOwner)
	, pScene(pInScene)
{
}

// デストラクタ
RenderComponent::~RenderComponent()
{
}
