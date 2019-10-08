#pragma once
#include <Windows.h>
#include "Vector2D.h"
#include <chrono>
#include <string>
#include "ProgramCore.h"

class GameObject
{
protected:
	bool GetKey(WPARAM keyName);
	bool GetMouseKey(int E_Mouse_Btn);
	Vector2D<int> getMousePos();
	std::chrono::duration<float> getDeltaTime();

	template<typename T>
	T* requireResource(std::string key)
	{
		return ProgramCore::instance.getCurScene()->requireResource<T>(key);
	}
	
public:
	virtual void Start() {};
	virtual void Update() {};
	virtual void LateUpdate() {};
	virtual ~GameObject() {};
	bool isActive = true;
};