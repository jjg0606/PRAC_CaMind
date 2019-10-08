#pragma once
#include "Vector2D.h"
#include <vector>
#include <Windows.h>
#include "Camera.h"
#include "Line.h"

class Collider
{
private:
	std::vector<Vector2D<int>>& getVertexInfo();
	bool MinMaxTest(Collider& dest);
	bool InclusionTest(Collider& dest);
	bool IsIncluded(Vector2D<int> pos);
	bool MeetTest(Collider& dest);
	bool ContinuTest(Collider& dest);
protected:
	std::vector<Vector2D<int>> vertexInfo;
	void SetRectCollider(int left, int top, int right, int bottom);
	void MoveDelta(int dx, int dy);
	void MoveTo(int left, int top);
	Vector2D<int> deltaVec{ 0,0 };
	bool isContinuous = false;
public:
	Vector2D<int> getDelta();
	virtual bool getMinMax(int& xmin, int& xmax, int& ymin, int& ymax);
	bool isActive = true;
	virtual void DrawCollider(HDC hdc);
	int Tag;
	virtual void ChkCollision(Collider& dest);
	virtual void OnCollision(Collider* destCol) {};
	virtual void SendMsg(int code) {};
	virtual ~Collider() {};
};