#ifndef COMPONENT_H
#define COMPONENT_H

class Actor;

// �A�N�^�[�ɕt��Component���N���X
class Component
{
public:

	// �R���X�g���N�^
	Component(Actor* pInOwner);

	// �f�X�g���N�^
	virtual ~Component() = 0;

	// �X�V
	virtual void Update() = 0;

protected:

	// ���L�Ҏ擾
	const Actor* GetOwner() const { return pOwner; }

private:

	// ���L��
	Actor* pOwner;
};

#endif		// #ifndef COMPONENT_H

