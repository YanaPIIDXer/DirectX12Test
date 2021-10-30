#include "Actor.h"
#include "Scene.h"

// コンストラクタ
Actor::Actor(Scene* pInScene)
	: pScene(pInScene)
{
	pScene->AddActor(this);
}

// デストラクタ
Actor::~Actor()
{
	pScene->RemoveActor(this);
}

// 更新
void Actor::Update()
{
	for (auto pComponent : components)
	{
		pComponent->Update();
	}
	Tick();
}
