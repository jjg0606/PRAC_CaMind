#pragma once
#include <Windows.h>
#include <unordered_map>
#include <string>
#include "Camera.h"

class Scene
{
protected:
	Camera* mainCam = nullptr;
	std::unordered_map<std::string, void*> resourceMap;
	virtual void Invalidate();
	RECT& getWinRect();
public:
	/// copy
	virtual void DrawScreen(HDC hdc) {}
	virtual void OnInput(WPARAM wParam) {}
	virtual void Update() {}
	virtual void Release() {}
	virtual void Init() {}
	virtual void OnMouseClick(int x, int y, int E_MOUSE_BTN) {}
	virtual void OnMenuMessage(WPARAM wParam) {}
	virtual void OnMouseMove() {}
	virtual void OnSocketEvent(UINT iMessage, WPARAM wParam, LPARAM lParam) {}
	/// copy end

	virtual ~Scene() {}
	virtual Camera* getMainCam();

	template<typename T>
	T* requireResource(std::string key)
	{
		return (T*)resourceMap[key];
	}
};