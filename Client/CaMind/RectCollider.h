#pragma once

#include "Collider.h"
#include "RectRenderer.h"

class RectCollider
	: virtual public Collider
{
protected:
	RectRenderer* render = nullptr;
	//std::vector<Vector2D<int>> vertexInfo;
	//void SetRectCollider(int left, int top, int right, int bottom);
	//void MoveDelta(int dx, int dy);
	//void MoveTo(int left, int top);
public:
	virtual bool getMinMax(int& xmin, int& xmax, int& ymin, int& ymax) override;
	//bool isActive = true;
	virtual void DrawCollider(HDC hdc) override;
	virtual void SetWidthHeight(int width, int height);
	//int Tag;
	//virtual bool isCollide(Collider& dest, int& xposOut, int& yposOut);
	//virtual void OnCollision(int destTag, int xpos, int ypos) {};
	RectCollider(int left, int top, int width, int height,int tag);
	void Init(int left, int top, int width, int height, int tag);
	void Init(int left, int top, int width, int height, int tag, bool isContinuous);
	RectCollider() = default;
	~RectCollider();
};
