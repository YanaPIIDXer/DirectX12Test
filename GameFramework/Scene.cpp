#include "Scene.h"
#include "Actor.h"

// �R���X�g���N�^
Scene::Scene()
{
}

// �f�X�g���N�^
Scene::~Scene()
{
}

// ���t���[���̏���
void Scene::Tick()
{
	Update();
}

// �A�N�^�[���폜
void Scene::RemoveActor(Actor *pActor)
{
	for (auto it = actors.begin(); it != actors.end(); ++it)
	{
		if (it->get() == pActor)
		{
			actors.erase(it);
			return;
		}
	}
}

// �`��
void Scene::Render(ID3D12GraphicsCommandList* pCommandList)
{
}
