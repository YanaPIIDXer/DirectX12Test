#include "RenderComponent.h"
#include "Scene.h"

// �R���X�g���N�^
RenderComponent::RenderComponent(Actor *pInOwner, Scene* pInScene)
	: Component(pInOwner)
	, pScene(pInScene)
{
	pScene->AddRenderComponent(this);
}

// �f�X�g���N�^
RenderComponent::~RenderComponent()
{
	pScene->RemoveRenderComponent(this);
}
