#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include "Actor.h"

class ID3D12GraphicsCommandList;
class RenderComponent;

// シーン基底クラス
class Scene {

public:

	// コンストラクタ
	Scene();

	// デストラクタ
	virtual ~Scene() = 0;

	// 毎フレームの処理
	void Tick();

	// 描画
	void Render(ID3D12GraphicsCommandList *pCommandList);

	// アクターを追加
	void AddActor(Actor* pActor)
	{
		actors.push_back(std::shared_ptr<Actor>(pActor));
	}

	// アクターを消去
	void RemoveActor(Actor* pActor);

	// 描画Componentを追加
	void AddRenderComponent(RenderComponent* pRenderComponent)
	{
		renderComponents.push_back(pRenderComponent);
	}

	// 描画Componentを削除
	void RemoveRenderComponent(RenderComponent* pRenderComponent)
	{
		for (auto it = renderComponents.begin(); it != renderComponents.end(); ++it)
		{
			if (*it == pRenderComponent)
			{
				renderComponents.erase(it);
				return;
			}
		}
	}

private:

	// アクターリスト
	std::vector<std::shared_ptr<Actor>> actors;

	// 描画Componentリスト
	std::vector<RenderComponent*> renderComponents;
};

#endif	// #ifndef SCENE_H
