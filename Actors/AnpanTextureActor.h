#ifndef ANPANTEXTUREACTOR_H
#define ANPANTEXTUREACTOR_H

#include "../GameFramework/Actor.h"

// �A���p���e�N�X�`����`�悷�邾����Actor
class AnpanTextureActor : public Actor
{
public:
	// �R���X�g���N�^
	AnpanTextureActor(Scene* pInScene)
		: Actor(pInScene) {}

	// �f�X�g���N�^
	virtual ~AnpanTextureActor() {}

	// ������
	virtual void Initialize(ID3D12Device* pDevice) override;
};

#endif      // #ifndef ANPANTEXTUREACTOR_H
