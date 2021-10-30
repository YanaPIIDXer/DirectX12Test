#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include "Component.h"

class Scene;
class ID3D12GraphicsCommandList;

// �`��Component
class RenderComponent : public Component
{
public:

	// �R���X�g���N�^
	RenderComponent(Actor *pInOwner, Scene* pInScene);

	// �f�X�g���N�^
	virtual ~RenderComponent() = 0;

	// �`��
	virtual void Render(ID3D12GraphicsCommandList* pCommandList) = 0;

private:

	// ���L�V�[��
	Scene* pScene;
};

#endif		// #ifndef RENDERCOMPONENT_H
