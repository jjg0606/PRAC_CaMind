#include "DrawObject.h"
#include "EnumMouse.h"
#include "PACKET.h"
using namespace std;

extern SOCKET g_sock;

void DrawObject::Update()
{
	drawSync -= getDeltaTime().count();
	if (drawSync < 0 && pointVec.size() != SyncedSize)
	{
		drawSync = 0.1f;
		PACKET_GAME_POINTS pointPac;
		pointPac.header.type = PACKET_TYPE_GAME_POINTS;
		pointPac.pointNum = pointVec.size() - SyncedSize >= 20 ? 20 : pointVec.size() - SyncedSize;
		int start = SyncedSize;
		int end = SyncedSize + pointPac.pointNum;
		int idx = 0;
		for (int i = start; i < end; i++, idx++)
		{
			pointPac.point[2*idx] = pointVec[i].first;
			pointPac.point[2*idx+1] = pointVec[i].second;
		}
		pointPac.header.size = sizeof(PACKET_HEADER) + sizeof(int) + sizeof(int) * 2 * pointPac.pointNum;
		send(g_sock, (const char*)&pointPac, pointPac.header.size, 0);
		SyncedSize = end;
	}
}

void DrawObject::Render(HDC hdc)
{
	bool moveTo = false;
	for (int i = 0; i < pointVec.size(); i++)
	{
		if (moveTo && pointVec[i].first >= 0)
		{
			MoveToEx(hdc, pointVec[i].first, pointVec[i].second, NULL);
			moveTo = false;
		}
		else if (pointVec[i].first < 0)
		{
			moveTo = true;
		}
		else if(pointVec[i].first > 0)
		{
			LineTo(hdc, pointVec[i].first, pointVec[i].second);
		}
	}
}

void DrawObject::PushPoint(int x, int y)
{
	pointVec.push_back(make_pair(x, y));
}

int DrawObject::getPointSize()
{
	return pointVec.size();
}

void DrawObject::sendNewPoint()
{

}

void DrawObject::Clear()
{
	SyncedSize = 0;
	drawSync = 0.1f;
	pointVec.clear();
}