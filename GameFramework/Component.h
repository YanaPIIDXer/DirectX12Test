#ifndef COMPONENT_H
#define COMPONENT_H

class Actor;

// アクターに付くComponent基底クラス
class Component
{
public:

	// コンストラクタ
	Component(Actor* pInOwner);

	// デストラクタ
	virtual ~Component() = 0;

	// 更新
	virtual void Update() = 0;

protected:

	// 所有者取得
	const Actor* GetOwner() const { return pOwner; }

private:

	// 所有者
	Actor* pOwner;
};

#endif		// #ifndef COMPONENT_H

