#include "Component.h"

// コンストラクタ
Component::Component(Actor* pInOwner)
	: pOwner(pInOwner)
{
}

// デストラクタ
Component::~Component()
{
}
