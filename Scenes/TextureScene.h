#ifndef TEXTURESCENE_H
#define TEXTURESCENE_H
#include "../GameFramework/Scene.h"

// テクスチャ描画シーン
class TextureScene : public Scene
{
public:

	// 初期化
	virtual void Initialize(ID3D12Device* pDevice) override;
};

#endif		// #ifndef TEXTURE_SCENE
