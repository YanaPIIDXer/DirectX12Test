#ifndef ACTOR_H
#define ACTOR_H

#include <vector>
#include <memory>
#include "Component.h"

class Scene;

// �A�N�^�[�N���X
class Actor
{
public:
	// �R���X�g���N�^
	Actor(Scene *pInScene);

	// �f�X�g���N�^
	virtual ~Actor() = 0;

	// �X�V
	void Update();

protected:

	// �R���|�[�l���g�ǉ�
	void AddComponent(Component* pComponent)
	{
		components.push_back(std::shared_ptr<Component>(pComponent));
	}

	// �X�V
	virtual void Tick() {}

private:

	// �V�[��
	Scene* pScene;

	// �R���|�[�l���g���X�g
	std::vector<std::shared_ptr<Component>> components;
};

#endif		// #ifndef ACTOR_H
