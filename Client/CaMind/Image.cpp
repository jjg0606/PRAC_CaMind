#include "Image.h"

using namespace std;

Image::Image(wstring path)
{
	this->handle = (HBITMAP)LoadImage(NULL, path.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	BITMAP info;
	GetObject(handle, sizeof(BITMAP), &info);

	bmWidth = info.bmWidth;
	bmHeight = info.bmHeight;
}

Image::~Image()
{
	DeleteObject(handle);
}