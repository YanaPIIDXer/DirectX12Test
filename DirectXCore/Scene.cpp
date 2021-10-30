#include "Scene.h"
#include "Renderable.h"

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

// �`��
void Scene::Render(ID3D12GraphicsCommandList* pCommandList)
{
	for (auto* pRenderComponent : renderComponents)
	{
		pRenderComponent->Render(pCommandList);
	}
}
