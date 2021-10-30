#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <memory>

class Actor;
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

	// アクターを追加
	void AddActor(Actor* pActor)
	{
		actors.push_back(std::shared_ptr<Actor>(pActor));
	}

	// アクターを消去
	void RemoveActor(Actor* pActor);

private:

	// アクターリスト
	std::vector<std::shared_ptr<Actor>> actors;
};

#endif	// #ifndef SCENE_H
