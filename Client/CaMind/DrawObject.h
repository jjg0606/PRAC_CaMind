#pragma once
#include "GameObject.h"
#include "Renderer.h"
#include <vector>
#include <list>
#include <map>

class DrawObject
	: virtual public GameObject, virtual public Renderer
{
	//std::list<std::pair<int, int>> pointVec;
	std::vector<std::pair<int, int>> pointVec;
	int SyncedSize = 0;
	bool isContinuous = false;
	float drawSync = 0.1f;
public:

	void Update() override;
	void Render(HDC hdc) override;
	void PushPoint(int x, int y);
	int getPointSize();
	void sendNewPoint();
	void Clear();
};