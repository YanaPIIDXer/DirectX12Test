#ifndef ACTOR_H
#define ACTOR_H

#include <vector>
#include <memory>
#include "Component.h"

struct ID3D12Device;
class Scene;

// �A�N�^�[�N���X
class Actor
{
public:
	// �R���X�g���N�^
	Actor(Scene *pInScene);

	// �f�X�g���N�^
	virtual ~Actor() = 0;

	// ������
	virtual void Initialize(ID3D12Device* pDevice) {}

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

	// �V�[���擾
	Scene* GetScene() const { return pScene; }

private:

	// �V�[��
	Scene* pScene;

	// �R���|�[�l���g���X�g
	std::vector<std::shared_ptr<Component>> components;
};

#endif		// #ifndef ACTOR_H
