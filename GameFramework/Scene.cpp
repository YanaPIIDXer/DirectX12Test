#include "Scene.h"
#include "Actor.h"

// コンストラクタ
Scene::Scene()
{
}

// デストラクタ
Scene::~Scene()
{
}

// 毎フレームの処理
void Scene::Tick()
{
	Update();
}

// アクターを削除
void Scene::RemoveActor(Actor *pActor)
{
	for (auto it = actors.begin(); it != actors.end(); ++it)
	{
		if (it->get() == pActor)
		{
			actors.erase(it);
			return;
		}
	}
}

// 描画
void Scene::Render(ID3D12GraphicsCommandList* pCommandList)
{
}
