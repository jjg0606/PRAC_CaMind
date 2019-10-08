#include "ImageRenderer.h"

ImageRenderer::ImageRenderer(Image* img, UINT transparent,int zIndex)
{
	this->img = img;
	this->transparent = transparent;
	position.x = 0;
	position.y = 0;
	Renderer::zIndex = zIndex;
}

void ImageRenderer::Init(Image* img, UINT transparent,int zIndex)
{
	this->img = img;
	this->transparent = transparent;
	position.x = 0;
	position.y = 0;
	Renderer::zIndex = zIndex;
}

void ImageRenderer::SetPosition(int x, int y)
{
	position.x = x;
	position.y = y;
}

void ImageRenderer::SetImg(Image* img)
{
	this->img = img;
}

void ImageRenderer::SetTransparent(UINT rgb)
{
	this->transparent = rgb;
}

void ImageRenderer::Render(HDC hdc)
{
	Camera* maincam = getMainCam();

	int xleft = position.x;
	int ytop = position.y;

	if (xleft > maincam->right || xleft + img->bmWidth < maincam->left
		|| ytop < maincam->bottom || ytop + img->bmHeight < maincam->top)
	{
		return;
	}

	xleft -= maincam->left;
	ytop -= maincam->top;

	SelectObject(getMemDC(), img->handle);
	TransparentBlt(hdc, xleft, ytop, img->bmWidth, img->bmHeight, getMemDC(), 0, 0, img->bmWidth, img->bmHeight, transparent);

}