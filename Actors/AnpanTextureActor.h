#ifndef ANPANTEXTUREACTOR_H
#define ANPANTEXTUREACTOR_H

#include "../GameFramework/Actor.h"

// アンパンテクスチャを描画するだけのActor
class AnpanTextureActor : public Actor
{
public:
	// コンストラクタ
	AnpanTextureActor(Scene* pInScene)
		: Actor(pInScene) {}

	// デストラクタ
	virtual ~AnpanTextureActor() {}

	// 初期化
	virtual void Initialize(ID3D12Device* pDevice) override;
};

#endif      // #ifndef ANPANTEXTUREACTOR_H
