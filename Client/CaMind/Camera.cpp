#include "Camera.h"

Camera::Camera(int left, int top, int width, int height)
	: left(left), top(top),right(left+width),bottom(top+bottom)
{
	leftEnd = -2000000000;
	rightEnd = 2000000000;
	topEnd = -2000000000;
	bottomEnd = 2000000000;
}

void Camera::Init(int left, int top, int width, int height)
{
	leftEnd = -2000000000;
	rightEnd = 2000000000;
	topEnd = -2000000000;
	bottomEnd = 2000000000;
	this->left = left;
	this->top = top;
	this->right = left + width;
	this->bottom = top + height;
}

void Camera::chkEndPoint()
{
	int width = right - left;
	int height = bottom - top;
	if (left < leftEnd)
	{
		left = leftEnd;
		right = left + width;
	}
	if (right > rightEnd)
	{
		right = rightEnd;
		left = right - width;
	}
	if (top < topEnd)
	{
		top = topEnd;
		bottom = top + height;
	}
	if (bottom > bottomEnd)
	{
		bottom = bottomEnd;
		top = bottom - height;
	}
}

void Camera::MoveTo(int x, int y)
{
	int width = right - left;
	int height = bottom - top;

	left = x;
	right = x + width;
	top = y;
	bottom = y + height;
}

void Camera::MoveDelta(int dx, int dy)
{
	left += dx;
	right += dx;
	top += dy;
	bottom += dy;
}

void Camera::SetEndPoint(int leftEnd, int rightEnd, int topEnd, int bottomEnd)
{
	this->leftEnd = leftEnd;
	this->rightEnd = rightEnd;
	this->topEnd = topEnd;
	this->bottomEnd = bottomEnd;
}