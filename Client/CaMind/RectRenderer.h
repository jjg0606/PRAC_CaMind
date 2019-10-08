#pragma once

#include "Renderer.h"
#include "Vector2D.h"

class RectRenderer
	: virtual public Renderer
{
protected :
	Vector2D<int> position{ 0,0 };
	int width;
	int height;
public:
	void Render(HDC hdc) override;
	void SetPosition(int x, int y);
	void SetWidthHeight(int width, int height);
};