#include "Renderer.h"
#include "ProgramCore.h"

Camera* Renderer::getMainCam()
{
	return ProgramCore::instance.getCurScene()->getMainCam();
}

HDC Renderer::getMemDC()
{
	return ProgramCore::instance.getMemDC();
}