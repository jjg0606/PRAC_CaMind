#include "DrawObject.h"
#include "EnumMouse.h"
#include "PACKET.h"
using namespace std;

extern SOCKET g_sock;

void DrawObject::Update()
{
	/*Vector2D<int>&& mousePos = getMousePos();
	if (GetMouseKey(MOUSE_LEFT_BTN))
	{		
		if (isContinuous)
		{
			pointVec.push_back(make_pair(mousePos.x,mousePos.y));
		}
		else
		{
			isContinuous = true;
			pointVec.push_back(make_pair(-1, -1));
		}
	}
	else
	{
		if (isContinuous)
		{
			isContinuous = false;
		}
	}
	drawSync -= getDeltaTime().count();
	if (drawSync < 0 && !pointVec.empty())
	{
		drawSync = 0.1f;
		PACKET_POINT psend;
		psend.num = pointVec.size() < 20 ? pointVec.size() : 20;
		psend.header.type = PACKET_TYPE_POINT;
		psend.header.size = sizeof(psend);
		for (int i = 0; i < psend.num; i++)
		{
			psend.xarr[i] = pointVec.front().first;
			psend.yarr[i] = pointVec.front().second;
			pointVec.pop_front();
		}
		send(g_sock, (const char*)&psend, sizeof(psend), 0);
	}*/
}

void DrawObject::Render(HDC hdc)
{
	bool moveTo = false;
	for (int i = 0; i < pointVecFromServer.size(); i++)
	{
		if (moveTo && pointVecFromServer[i].first >= 0)
		{
			MoveToEx(hdc, pointVecFromServer[i].first, pointVecFromServer[i].second, NULL);
			moveTo = false;
		}
		else if (pointVecFromServer[i].first < 0)
		{
			moveTo = true;
		}
		else if(pointVecFromServer[i].first > 0)
		{
			LineTo(hdc, pointVecFromServer[i].first, pointVecFromServer[i].second);
		}
	}
}

void DrawObject::PushPoint(int x, int y)
{
	pointVecFromServer.push_back(make_pair(x, y));
}

int DrawObject::getPointSize()
{
	return pointVec.size();
}

void DrawObject::sendNewPoint()
{

}
