#include "Scene.h"
#include "ProgramCore.h"

void Scene::Invalidate()
{
	ProgramCore::instance.Invalidate();
}

Camera* Scene::getMainCam()
{
	return mainCam;
}

RECT& Scene::getWinRect()
{
	return ProgramCore::instance.getWinRect();
}