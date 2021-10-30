#ifndef ACTOR_H
#define ACTOR_H

#include <vector>

class Scene;
class Component;

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
		components.push_back(pComponent);
	}

	// �X�V
	virtual void Tick() {}

private:

	// �V�[��
	Scene* pScene;

	// �R���|�[�l���g���X�g
	std::vector<Component*> components;
};

#endif		// #ifndef ACTOR_H
