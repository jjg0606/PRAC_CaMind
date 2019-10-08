#include "GameObject.h"


Vector2D<int> GameObject::getMousePos()
{
	return ProgramCore::instance.getMousePos();
}

std::chrono::duration<float> GameObject::getDeltaTime()
{
	return ProgramCore::instance.getDeltaTime();
}

bool GameObject::GetKey(WPARAM keyName)
{
	return ProgramCore::instance.GetKey(keyName);
}


bool GameObject::GetMouseKey(int E_Mouse_btn)
{
	return ProgramCore::instance.GetMouseKey(E_Mouse_btn);
}