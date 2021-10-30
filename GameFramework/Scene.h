#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>
#include "Actor.h"

class ID3D12GraphicsCommandList;
class RenderComponent;

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

	// �`��Component��ǉ�
	void AddRenderComponent(RenderComponent* pRenderComponent)
	{
		renderComponents.push_back(pRenderComponent);
	}

	// �`��Component���폜
	void RemoveRenderComponent(RenderComponent* pRenderComponent)
	{
		for (auto it = renderComponents.begin(); it != renderComponents.end(); ++it)
		{
			if (*it == pRenderComponent)
			{
				renderComponents.erase(it);
				return;
			}
		}
	}

private:

	// �A�N�^�[���X�g
	std::vector<std::shared_ptr<Actor>> actors;

	// �`��Component���X�g
	std::vector<RenderComponent*> renderComponents;
};

#endif	// #ifndef SCENE_H
