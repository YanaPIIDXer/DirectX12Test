#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>

class Actor;
class ID3D12GraphicsCommandList;

// �V�[�����N���X
class Scene {

public:

	// �R���X�g���N�^
	Scene();

	// �f�X�g���N�^
	virtual ~Scene() = 0;

	// ���t���[���̏���
	void Tick();

	// �`��
	void Render(ID3D12GraphicsCommandList *pCommandList);

	// �A�N�^�[��ǉ�
	void AddActor(Actor* pActor)
	{
		actors.push_back(std::shared_ptr<Actor>(pActor));
	}

	// �A�N�^�[������
	void RemoveActor(Actor* pActor);

private:

	// �A�N�^�[���X�g
	std::vector<std::shared_ptr<Actor>> actors;
};

#endif	// #ifndef SCENE_H
