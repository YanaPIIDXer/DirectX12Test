#ifndef SCENE_H
#define SCENE_H

#include <vector>
class IRenderable;
class ID3D12GraphicsCommandList;

// シーン基底クラス
class Scene {

public:

	// コンストラクタ
	Scene();

	// デストラクタ
	virtual ~Scene() = 0;

	// 毎フレームの処理
	void Tick();

	// 描画
	void Render(ID3D12GraphicsCommandList *pCommandList);

	// 描画用Componentを追加
	void AddRenderComponent(IRenderable* pRenderableComponent)
	{
		renderComponents.push_back(pRenderableComponent);
	}

protected:

	// 更新処理
	virtual void Update() = 0;

private:

	// 描画Componentリスト
	std::vector<IRenderable*> renderComponents;
};

#endif	// #ifndef SCENE_H
