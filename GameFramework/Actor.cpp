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
