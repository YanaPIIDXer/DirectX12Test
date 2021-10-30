#ifndef RENDERABLE_H
#define RENDERABLE_H

// �`��\Component�Ɏ�������C���^�t�F�[�X
class IRenderable
{
public:
	// �`��
	virtual void Render() = 0;

	virtual ~IRenderable() = 0;
};

#endif		// #ifndef RENDERABLE_H
