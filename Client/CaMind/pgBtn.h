#pragma once

class pgBtn
{
public:
	int xpos;
	int ypos;
	int width;
	int height;

	pgBtn(int xpos, int ypos, int width, int height)
		: xpos(xpos), ypos(ypos), width(width), height(height)
	{

	}

	bool isIn(int x, int y)
	{
		if (xpos <= x && x <= xpos + width && ypos <= y && y <= ypos + height)
		{
			return true;
		}
		return false;
	}
};