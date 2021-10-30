#ifndef ACTOR_H
#define ACTOR_H

class Scene;

// アクタークラス
class Actor
{
public:
	// コンストラクタ
	Actor(Scene *pScene);

	// デストラクタ
	virtual ~Actor() = 0;
};

#endif		// #ifndef ACTOR_H
