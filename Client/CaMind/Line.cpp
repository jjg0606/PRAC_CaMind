#include "Line.h"
#include <math.h>
#include <Windows.h>

using namespace std;

Line::Line(Vector2D<int> p1, Vector2D<int> p2)
{
	if (p1.x == p2.x)
	{
		yparam = 0.0f;
		xparam = 1.0f;
		cons = -p1.x;
		start = min(p1.y, p2.y);
		end = max(p1.y, p2.y);

		return;
	}

	yparam = 1.0f;
	start = min(p1.x, p2.x);
	end = max(p1.x, p2.x);
	xparam = (p2.y - p1.y) / (float)(p2.x - p1.x);
	cons = (float)p1.y - xparam * (float)p1.x;
}

// TODO
bool Line::isMeet(Line& dest)
{
	if (this->yparam == 0.0f)
	{
		if (dest.yparam = 0.0f)
		{
			return resultYParalEach(dest);
		}
		else
		{
			return resultYParalOne(dest);
		}
	}
	else if (dest.yparam == 0.0f)
	{
		return dest.resultYParalOne(*this);
	}
	   
	return resultOrdinary(dest);
}

bool Line::resultYParalEach(Line& dest)
{
	if (this->cons != dest.cons)
	{
		return false;
	}
	if (this->end < dest.start || dest.end < this->start)
	{
		return false;
	}
	return true;
}


bool Line::resultYParalOne(Line& dest)
{
	// this is pararell with Y line
	// but dest is not
	if (-this->cons < dest.start || -this->cons > dest.end)
	{
		return false;
	}
	
	float consY = dest.getY(this->cons);

	if (consY < this->start || consY > this->end)
	{
		return false;
	}
	return true;
}



bool Line::resultOrdinary(Line& dest)
{
	if (this->end < dest.start || dest.end < this->start)
	{
		return false;
	}
	// pararell line each
	if (this->xparam == dest.xparam)
	{
		if (this->cons == dest.cons)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	float meetPoint = -1 * (this->cons - dest.cons) / (this->xparam - dest.xparam);

	if (meetPoint < this->start || meetPoint > this->end || meetPoint < dest.start || meetPoint > dest.end)
	{
		return false;
	}

	return true;
}

float Line::getY(float X)
{
	return xparam * X + cons;
}