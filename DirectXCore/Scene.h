#ifndef SCENE_H
#define SCENE_H

#include <vector>
class IRenderable;
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

	// �`��pComponent��ǉ�
	void AddRenderComponent(IRenderable* pRenderableComponent)
	{
		renderComponents.push_back(pRenderableComponent);
	}

protected:

	// �X�V����
	virtual void Update() = 0;

private:

	// �`��Component���X�g
	std::vector<IRenderable*> renderComponents;
};

#endif	// #ifndef SCENE_H
