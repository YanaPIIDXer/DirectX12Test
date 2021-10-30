#include "Actor.h"
#include "Scene.h"

// �R���X�g���N�^
Actor::Actor(ID3D12Device* pDevice, Scene* pInScene)
	: pScene(pInScene)
{
	pScene->AddActor(this);
}

// �f�X�g���N�^
Actor::~Actor()
{
	pScene->RemoveActor(this);
}

// �X�V
void Actor::Update()
{
	for (auto pComponent : components)
	{
		pComponent->Update();
	}
	Tick();
}
