#pragma once

class Camera
{
private:
	void chkEndPoint();
public:
	int left;
	int top;
	int right;
	int bottom;

	int leftEnd;
	int rightEnd;
	int topEnd;
	int bottomEnd;

	Camera(int left, int top, int width, int height);
	Camera() = default;
	void Init(int left, int top, int width, int height);
	void MoveTo(int x, int y);
	void MoveDelta(int dx, int dy);
	void SetEndPoint(int leftEnd, int rightEnd, int topEnd, int bottomEnd);
};
