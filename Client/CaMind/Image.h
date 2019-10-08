#pragma once
#include <Windows.h>
#include <string>

class Image
{
public:
	HBITMAP handle;
	int bmWidth;
	int bmHeight;
	Image(std::wstring);
	~Image();
};