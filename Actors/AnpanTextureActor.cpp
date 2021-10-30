#include "AnpanTextureActor.h"
#include "../Components/TextureComponent.h"

// ������
void AnpanTextureActor::Initialize(ID3D12Device* pDevice)
{
	TextureComponent* pTexComponent = new TextureComponent(this, GetScene());
	pTexComponent->LoadTexture(pDevice, L"imgs/Anpan.jpg");
	AddComponent(pTexComponent);
}
