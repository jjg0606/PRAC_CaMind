#include "Collider.h"
#include <math.h>
#include "Line.h"
using namespace std;

std::vector<Vector2D<int>>& Collider::getVertexInfo()
{
	return vertexInfo;
}

bool Collider::getMinMax(int& xmin, int& xmax, int& ymin, int& ymax)
{
	if (vertexInfo.empty())
	{
		return false;
	}

	xmin = vertexInfo[0].x;
	xmax = vertexInfo[0].x;
	ymin = vertexInfo[0].y;
	ymax = vertexInfo[0].y;

	for (int i = 1; i < vertexInfo.size(); i++)
	{
		xmin = min(xmin, vertexInfo[i].x);
		xmax = max(xmax, vertexInfo[i].x);
		ymin = min(ymin, vertexInfo[i].y);
		ymax = max(ymax, vertexInfo[i].y);
	}
	return true;
}

bool Collider::MinMaxTest(Collider& dest)
{
	int sorXmin, sorXmax, sorYmin, sorYmax;
	if (!getMinMax(sorXmin, sorXmax, sorYmin, sorYmax))
	{
		return false;
	}

	int destXmin, destXmax, destYmin, destYmax;
	if (!dest.getMinMax(destXmin, destXmax, destYmin, destYmax))
	{
		return false;
	}

	if (sorXmax < destXmin || destXmax < sorXmin || sorYmax < destYmin || destYmax < sorYmin)
	{
		return false;
	}

	return true;
}

bool Collider::MeetTest(Collider& dest)
{
	for (int i = 0; i < vertexInfo.size(); i++)
	{
		int inext = (i + 1) % vertexInfo.size();
		Line lorigin(vertexInfo[i], vertexInfo[inext]);
		for (int j = 0; j < dest.vertexInfo.size(); j++)
		{
			int jnext = (j + 1) % dest.vertexInfo.size();
			Line ldest(dest.vertexInfo[j], dest.vertexInfo[jnext]);
			if (lorigin.isMeet(ldest))
			{
				this->OnCollision(&dest);
				dest.OnCollision(this);
				return true;
			}
		}
	}
	return false;
}

bool Collider::ContinuTest(Collider& dest)
{
	Vector2D<int> delta = getDelta();
	for (int i = 0; i < vertexInfo.size(); i++)
	{		
		Line lorigin(vertexInfo[i], vertexInfo[i] - delta);

		for (int j = 0; j < dest.vertexInfo.size(); j++)
		{
			int jnext = j + 1 < dest.vertexInfo.size() ? j + 1 : 0;
			Line ldest(dest.vertexInfo[j], dest.vertexInfo[jnext]);
			if (lorigin.isMeet(ldest))
			{
				this->OnCollision(&dest);
				dest.OnCollision(this);
				return true;
			}
		}
	}
	return false;
}

bool Collider::InclusionTest(Collider& dest)
{
	for (int i = 0; i < dest.vertexInfo.size(); i++)
	{
		if (this->IsIncluded(dest.vertexInfo[i]))
		{
			this->OnCollision(&dest);
			dest.OnCollision(this);
			return true;
		}
	}	
	return false;
}


void Collider::SetRectCollider(int left, int top, int right, int bottom)
{
	vertexInfo.clear();
	vertexInfo.push_back({ left,top });
	vertexInfo.push_back({ right,top });
	vertexInfo.push_back({ right,bottom });
	vertexInfo.push_back({ left,bottom });
}

void Collider::MoveDelta(int dx, int dy)
{
	deltaVec = Vector2D<int>{ dx,dy };
	for (int i = 0; i < vertexInfo.size(); i++)
	{
		vertexInfo[i].x += dx;
		vertexInfo[i].y += dy;
	}
}

void Collider::MoveTo(int left, int top)
{
	if (vertexInfo.empty())
	{
		return;
	}

	int dx = left - vertexInfo[0].x;
	int dy = top - vertexInfo[0].y;
	MoveDelta(dx, dy);
}

void Collider::DrawCollider(HDC hdc)
{

}

void Collider::ChkCollision(Collider& dest)
{
	if (!this->isActive || !dest.isActive)
	{
		return;
	}



	if (!MinMaxTest(dest))
	{
		if (this->isContinuous && this->ContinuTest(dest))
		{
			return;
		}

		if (dest.isContinuous && dest.ContinuTest(*this))
		{
			return;
		}
		return;
	}

	if (InclusionTest(dest))
	{
		return;
	}

	//if (dest.InclusionTest(*this))
	//{
	//	return;
	//}

	if (MeetTest(dest))
	{
		return;
	}
	   	 

}




bool Collider::IsIncluded(Vector2D<int> pos)
{
	static const float pi = 3.141592f;
	float angleSum = 0.0f;
	for (int i = 0; i < vertexInfo.size(); i++)
	{
		int next = (i + 1) % vertexInfo.size();
		Vector2D<int> v1 = vertexInfo[i] - pos;
		Vector2D<int> v2 = vertexInfo[next] - pos;

		int inpro = v1 * v2;
		float size = sqrtf(v1.x * v1.x + v1.y * v1.y) * sqrtf(v2.x * v2.x + v2.y * v2.y);
		angleSum += acosf(inpro / size);
	}

	return angleSum > 1.9f * pi ? true : false;
}

Vector2D<int> Collider::getDelta()
{
	return deltaVec;
}