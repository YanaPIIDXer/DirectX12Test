#include "Scene.h"
#include "Renderable.h"

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

// 描画
void Scene::Render(ID3D12GraphicsCommandList* pCommandList)
{
	for (auto* pRenderComponent : renderComponents)
	{
		pRenderComponent->Render(pCommandList);
	}
}
