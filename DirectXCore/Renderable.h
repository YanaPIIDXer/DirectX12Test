#ifndef RENDERABLE_H
#define RENDERABLE_H

// 描画可能Componentに実装するインタフェース
class IRenderable
{
public:
	// 描画
	virtual void Render() = 0;

	virtual ~IRenderable() = 0;
};

#endif		// #ifndef RENDERABLE_H
