#include "RectRenderer.h"

void RectRenderer::Render(HDC hdc)
{
	Camera* maincam = getMainCam();
	int xleft = position.x;
	int ytop = position.y;

	if (xleft > maincam->right || xleft + width < maincam->left 
		|| ytop < maincam->bottom || ytop+height < maincam->top)
	{
		return;
	}

	xleft -= maincam->left;
	ytop -= maincam->top;

	MoveToEx(hdc, xleft, ytop, NULL);
	LineTo(hdc, xleft + width, ytop);
	LineTo(hdc, xleft + width, ytop + height);
	LineTo(hdc, xleft, ytop + height);
	LineTo(hdc, xleft, ytop);	
}

void RectRenderer::SetPosition(int x, int y)
{
	position.x = x;
	position.y = y;
}

void RectRenderer::SetWidthHeight(int width, int height)
{
	this->width = width;
	this->height = height;
}