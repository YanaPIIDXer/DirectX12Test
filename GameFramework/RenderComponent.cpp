#include "RenderComponent.h"
#include "Scene.h"

// �R���X�g���N�^
RenderComponent::RenderComponent(Actor *pInOwner, Scene* pInScene)
	: Component(pInOwner)
	, pScene(pInScene)
{
}

// �f�X�g���N�^
RenderComponent::~RenderComponent()
{
}
