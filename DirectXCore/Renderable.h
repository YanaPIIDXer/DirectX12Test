#ifndef RENDERABLE_H
#define RENDERABLE_H

class ID3D12GraphicsCommandList;

// �`��\Component�Ɏ�������C���^�t�F�[�X
class IRenderable
{
public:
	// �`��
	virtual void Render(ID3D12GraphicsCommandList* pCommandList) = 0;

	virtual ~IRenderable() = 0;
};

#endif		// #ifndef RENDERABLE_H
