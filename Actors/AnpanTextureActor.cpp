#include "AnpanTextureActor.h"
#include "../Components/TextureComponent.h"

// ������
void AnpanTextureActor::Initialize(ID3D12Device* pDevice)
{
	TextureComponent* texComponent = new TextureComponent(this, GetScene());
	texComponent->LoadTexture(pDevice, L"imgs/Anpan.jpg");
}
