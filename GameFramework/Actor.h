#ifndef ACTOR_H
#define ACTOR_H

#include <vector>
#include <memory>
#include "Component.h"

class ID3D12Device;
class Scene;

// アクタークラス
class Actor
{
public:
	// コンストラクタ
	Actor(Scene *pInScene);

	// デストラクタ
	virtual ~Actor() = 0;

	// 初期化
	virtual void Initialize(ID3D12Device* pDevice) {}

	// 更新
	void Update();

protected:

	// コンポーネント追加
	void AddComponent(Component* pComponent)
	{
		components.push_back(std::shared_ptr<Component>(pComponent));
	}

	// 更新
	virtual void Tick() {}

private:

	// シーン
	Scene* pScene;

	// コンポーネントリスト
	std::vector<std::shared_ptr<Component>> components;
};

#endif		// #ifndef ACTOR_H
