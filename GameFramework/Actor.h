#ifndef ACTOR_H
#define ACTOR_H

class Scene;

// �A�N�^�[�N���X
class Actor
{
public:
	// �R���X�g���N�^
	Actor(Scene *pScene);

	// �f�X�g���N�^
	virtual ~Actor() = 0;
};

#endif		// #ifndef ACTOR_H
