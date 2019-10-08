#include "cmGameScene.h"
#include "PACKET.h"
#include "EnumImg.h"
#define BUFSIZE 512

using namespace std;

extern SOCKET g_sock;

void cmGameScene::DrawScreen(HDC hdc)
{
	FillRect(hdc, &getWinRect(),(HBRUSH)GetStockObject(WHITE_BRUSH));
	dro.Render(hdc);
}

void cmGameScene::LoadImg()
{
	imgMap[IMG_LOBBY_DOOR] = new Image(L"res/door.bmp");
}

void cmGameScene::OnInput(WPARAM wParam)
{

}

void cmGameScene::Update()
{
	/*dro.Update();*/
	Invalidate();
}

void cmGameScene::Release()
{

}

void cmGameScene::Init()
{
	LoadImg();

	ProgramCore::instance.SetUpdateIntersec(0);
	Invalidate();
}

void cmGameScene::OnMouseClick(int x, int y, int E_MOUSE_BTN)
{

}

cmGameScene::~cmGameScene()
{

}

void cmGameScene::OnMouseMove()
{
	dro.Update();
}

void cmGameScene::OnSocketEvent(UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	int addrlen = 0;
	int retval = 0;

	if (WSAGETSELECTERROR(lParam))
	{
		int err_code = WSAGETSELECTERROR(lParam);
		return;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_READ:
	{
		retval = recv(wParam, readBuf + bufIdx, BUFSIZE - bufIdx, 0);
		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				return;
			}
		}
		else if (retval == 0)
		{
			exit(-1);
		}
		bufIdx += retval;
		ProcessPacketByteStream();
	}
	break;
	case FD_CLOSE:
		closesocket(wParam);
		break;
	}
}

void cmGameScene::ProcessPacketByteStream()
{
	while (true)
	{
		PACKET_HEADER header;
		memcpy(&header, readBuf, sizeof(header));

		if (bufIdx < header.size)
		{
			break;
		}

		ProcessPacketCaller(header.type, header.size);
		bufIdx -= header.size;
	}
	
}

void cmGameScene::ProcessPacketCaller(int type, int size)
{
	switch (state)
	{
	case cmGameState::INIT:
		ProcessPacket<cmGameState::INIT>(type, size);
		break;
	case cmGameState::LOBBY:
		ProcessPacket<cmGameState::LOBBY>(type, size);
		break;
	case cmGameState::IN_GAME:
		ProcessPacket<cmGameState::IN_GAME>(type, size);
		break;
	}
}

template<>
void cmGameScene::ProcessPacket<cmGameState::INIT>(int type, int size)
{

}

template<>
void cmGameScene::ProcessPacket<cmGameState::LOBBY>(int type, int size)
{

}

template<>
void cmGameScene::ProcessPacket<cmGameState::IN_GAME>(int type, int size)
{
	switch (type)
	{

	case PACKET_TYPE_POINT:
	{
		PACKET_POINT getpos;
		memcpy(&getpos, readBuf, size);
		for (int i = 0; i < getpos.num; i++)
		{
			dro.PushPoint(getpos.xarr[i], getpos.yarr[i]);
		}
	}
	break;

	}
}

void cmGameScene::SendToServer(void* source, int size)
{
	send(g_sock, (const char*)source, size, 0);
}