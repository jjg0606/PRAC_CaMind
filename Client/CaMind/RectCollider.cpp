#include "RectCollider.h"

RectCollider::RectCollider(int left, int top, int width, int height, int tag)
{
	RectCollider::Init(left, top, width, height, tag);
	/*Collider::Tag = tag;
	SetRectCollider(left, top, left + width, top + height);
	render = new RectRenderer();
	render->SetWidthHeight(width, height);*/
}

void RectCollider::Init(int left, int top, int width, int height, int tag)
{
	Collider::Tag = tag;
	SetRectCollider(left, top, left + width, top + height);

	if (render != nullptr)
	{
		render->SetPosition(left, top);
		render->SetWidthHeight(width, height);
	}
	else
	{
		render = new RectRenderer();
		render->SetWidthHeight(width, height);
	}

	
}

void RectCollider::Init(int left, int top, int width, int height, int tag, bool isContinuous)
{
	RectCollider::Init(left, top, width, height, tag);
	Collider::isContinuous = isContinuous;
}

bool RectCollider::getMinMax(int& xmin, int& xmax, int& ymin, int& ymax)
{
	if (vertexInfo.size() != 4)
	{
		return false;
	}

	xmin = vertexInfo[0].x;
	xmax = vertexInfo[2].x;
	ymin = vertexInfo[0].y;
	ymax = vertexInfo[2].y;
	return true;
}

void RectCollider::DrawCollider(HDC hdc)
{
	int xleft = vertexInfo[0].x;
	int ytop = vertexInfo[0].y;
	int width = vertexInfo[2].x - xleft;
	int height = vertexInfo[2].y - ytop;

	render->SetPosition(xleft, ytop);
	render->Render(hdc);
}

RectCollider::~RectCollider()
{
	delete render;
}

void RectCollider::SetWidthHeight(int width, int height)
{
	if (vertexInfo.size() != 4)
	{
		return;
	}

	vertexInfo[1] = vertexInfo[0] + Vector2D<int>{width, 0};
	vertexInfo[2] = vertexInfo[0] + Vector2D<int>{width, height};
	vertexInfo[3] = vertexInfo[0] + Vector2D<int>{0, height};
}

