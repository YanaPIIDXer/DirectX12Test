#include "TextureScene.h"
#include "../Actors/AnpanTextureActor.h"

// ‰Šú‰»
void TextureScene::Initialize(ID3D12Device* pDevice)
{
	AnpanTextureActor* pActor = new AnpanTextureActor(this);
	pActor->Initialize(pDevice);
	AddActor(pActor);
}
