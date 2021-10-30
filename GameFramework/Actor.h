#ifndef ACTOR_H
#define ACTOR_H

#include <vector>

class Scene;
class Component;

// アクタークラス
class Actor
{
public:
	// コンストラクタ
	Actor(Scene *pInScene);

	// デストラクタ
	virtual ~Actor() = 0;

	// 更新
	void Update();

protected:

	// コンポーネント追加
	void AddComponent(Component* pComponent)
	{
		components.push_back(pComponent);
	}

	// 更新
	virtual void Tick() {}

private:

	// シーン
	Scene* pScene;

	// コンポーネントリスト
	std::vector<Component*> components;
};

#endif		// #ifndef ACTOR_H
