#pragma once
#include "Vector2D.h"

// ax + b = y 의 형태
// a에 해당하는 변수 xparam
// b에 해당하는 변수 cons
// c에 해당하는 변수 yparam
// 정의역에 해당하는 변수 start end
class Line
{
private:

	// Yparal - not Yparal
	bool resultYParalOne(Line& dest);
	// Yparal - Yparal
	bool resultYParalEach(Line & dest);
	// Ordinary one
	bool resultOrdinary(Line& dest);
	float xparam;
	float cons;
	float yparam;
	int start;
	int end;
public:
	float getY(float X);
	Line(Vector2D<int> p1, Vector2D<int> p2);
	bool isMeet(Line& dest);
};