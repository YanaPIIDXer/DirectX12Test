#ifndef TEXTURESCENE_H
#define TEXTURESCENE_H
#include "../GameFramework/Scene.h"

// �e�N�X�`���`��V�[��
class TextureScene : public Scene
{
public:

	// ������
	virtual void Initialize(ID3D12Device* pDevice) override;
};

#endif		// #ifndef TEXTURE_SCENE
