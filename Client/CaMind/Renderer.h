#pragma once
#include <Windows.h>
#include "Camera.h"

class Renderer
{
protected:
	HDC getMemDC();
	Camera* getMainCam();
public:
	int zIndex;
	virtual void Render(HDC hdc) {};
};