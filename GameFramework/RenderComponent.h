#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include "Component.h"

class Scene;

// �`��Component
class RenderComponent : public Component
{
public:

	// �R���X�g���N�^
	RenderComponent(Actor *pInOwner, Scene* pInScene);

	// �f�X�g���N�^
	virtual ~RenderComponent() = 0;

	// �`��
	virtual void Render() = 0;

private:

	// ���L�V�[��
	Scene* pScene;
};

#endif		// #ifndef RENDERCOMPONENT_H
