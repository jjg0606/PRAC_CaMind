#include "cmGameScene.h"
#include "PACKET.h"
#include "EnumImg.h"
#include "EnumMouse.h"
#define BUFSIZE 512

using namespace std;

extern SOCKET g_sock;

void cmGameScene::DrawScreen(HDC hdc)
{
	FillRect(hdc, &getWinRect(),(HBRUSH)GetStockObject(WHITE_BRUSH));
	StateDrawCaller(hdc);
	dro.Render(hdc);
}

void cmGameScene::LoadImg()
{
	imgMap[IMG_LOBBY_DOOR] = new Image(L"res/door.bmp");
	imgMap[IMG_AURA_MY] = new Image(L"res/aura_my.bmp");
	imgMap[IMG_AURA_TURN] = new Image(L"res/aura_turn.bmp");
	imgMap[IMG_AVATAR_1] = new Image(L"res/avatar.bmp");
	imgMap[IMG_AVATAR_2] = new Image(L"res/avatar_egg.bmp");
	imgMap[IMG_AVATAR_3] = new Image(L"res/avatar_egg2.bmp");
	imgMap[IMG_AVATAR_4] = new Image(L"res/avatar_tomato.bmp");
	imgMap[IMG_AURA_READY] = new Image(L"res/avatar_on_ready.bmp");
	imgMap[IMG_BTN_CONFIRM] = new Image(L"res/btn_confirm.bmp");
	imgMap[IMG_LABEL_LOGIN] = new Image(L"res/label_login.bmp");
	imgMap[IMG_BTN_READY] = new Image(L"res/ready_btn.bmp");
	imgMap[IMG_BTN_REFRESH] = new Image(L"res/refresh_btn.bmp");
	imgMap[IMG_BTN_EXIT] = new Image(L"res/exit_btn.bmp");
}

void cmGameScene::OnInput(WPARAM wParam)
{

}

void cmGameScene::Update()
{
	/*dro.Update();*/
	StateUpdateCaller();
	Invalidate();
}

void cmGameScene::Release()
{
	StateReleaseCaller();
	for (auto iter = imgMap.begin(); iter != imgMap.end(); iter++)
	{
		delete iter->second;
	}
	imgMap.clear();
	delete mainCam;
}

void cmGameScene::Init()
{
	LoadImg();

	ProgramCore::instance.SetUpdateIntersec(0);
	Invalidate();
	
	mainCam = new Camera(0, 0, getWinRect().right, getWinRect().bottom);
	StateInitCaller();

	render.SetTransparent(RGB(255, 0, 255));
}

void cmGameScene::OnMouseClick(int x, int y, int E_MOUSE_BTN)
{
	StateClickCaller(x, y, E_MOUSE_BTN);
}

cmGameScene::~cmGameScene()
{

}

void cmGameScene::OnMouseMove()
{
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

#pragma region CALLER
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

void cmGameScene::StateDrawCaller(HDC hdc)
{
	switch (state)
	{
	case cmGameState::INIT:
		StateDraw<cmGameState::INIT>(hdc);
		break;
	case cmGameState::LOBBY:
		StateDraw<cmGameState::LOBBY>(hdc);
		break;
	case cmGameState::IN_GAME:
		StateDraw<cmGameState::IN_GAME>(hdc);
		break;

	}
}

void cmGameScene::StateInitCaller()
{
	switch (state)
	{
	case cmGameState::INIT:
		StateInit<cmGameState::INIT>();
		break;
	case cmGameState::LOBBY:
		StateInit<cmGameState::LOBBY>();
		break;
	case cmGameState::IN_GAME:
		StateInit<cmGameState::IN_GAME>();
		break;
	}
}

void cmGameScene::StateReleaseCaller()
{
	switch (state)
	{
	case cmGameState::INIT:
		StateRelease<cmGameState::INIT>();
		break;
	case cmGameState::LOBBY:
		StateRelease<cmGameState::LOBBY>();
		break;
	case cmGameState::IN_GAME:
		StateRelease<cmGameState::IN_GAME>();
		break;
	}
}

void cmGameScene::StateUpdateCaller()
{
	switch (state)
	{
	case cmGameState::INIT:
		StateUpdate<cmGameState::INIT>();
		break;
	case cmGameState::LOBBY:
		StateUpdate<cmGameState::LOBBY>();
		break;
	case cmGameState::IN_GAME:
		StateUpdate<cmGameState::IN_GAME>();
		break;
	}
}

void cmGameScene::StateClickCaller(int x, int y, int E_BTN)
{
	switch (state)
	{
	case cmGameState::INIT:
		StateClick<cmGameState::INIT>(x, y, E_BTN);
		break;
	case cmGameState::LOBBY:
		StateClick<cmGameState::LOBBY>(x, y, E_BTN);
		break;
	case cmGameState::IN_GAME:
		StateClick<cmGameState::IN_GAME>(x, y, E_BTN);
		break;
	}
}
#pragma endregion

#pragma region STATE - INIT
template<>
void cmGameScene::ProcessPacket<cmGameState::INIT>(int type, int size)
{

}

template<>
void cmGameScene::StateDraw<cmGameState::INIT>(HDC hdc)
{
	render.SetImg(imgMap[IMG_LABEL_LOGIN]);
	render.SetPosition(50, 50);
	render.Render(hdc);

	for (int i = 0; i < 4; i++)
	{
		render.SetImg(imgMap[IMG_AVATAR_1 + i]);
		render.SetPosition(avatarBtn[i]->xpos, avatarBtn[i]->ypos);
		render.Render(hdc);

		if (selectedAvatar == i)
		{
			render.SetImg(imgMap[IMG_AURA_MY]);
			render.Render(hdc);
		}
	}

	render.SetImg(imgMap[IMG_BTN_CONFIRM]);
	render.SetPosition(confirmBtn->xpos, confirmBtn->ypos);
	render.Render(hdc);

}

template<>
void cmGameScene::StateInit<cmGameState::INIT>()
{
	int width = imgMap[IMG_AVATAR_1]->bmWidth;
	int height = imgMap[IMG_AVATAR_1]->bmHeight;
	int startx = 80;
	int starty = 200;
	int dx = 40;
	for (int i = 0; i < 4; i++)
	{
		avatarBtn[i] = new pgBtn(startx + (width + dx) * i, starty, width, height);
	}
	confirmBtn = new pgBtn(150, 400, imgMap[IMG_BTN_CONFIRM]->bmWidth, imgMap[IMG_BTN_CONFIRM]->bmHeight);

	selectedAvatar = 0;

	hName = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL, 150, 350, 300, 30, ProgramCore::instance.getHWND(), (HMENU)idName, ProgramCore::instance.getGinst(), NULL);
	SendMessage(hName, EM_LIMITTEXT, (WPARAM)MAX_NAME_LENGTH, 0);
}

template<>
void cmGameScene::StateRelease<cmGameState::INIT>()
{
	for (int i = 0; i < 4; i++)
	{
		delete avatarBtn[i];
	}
	delete confirmBtn;

	DestroyWindow(hName);
}

template<>
void cmGameScene::StateUpdate<cmGameState::INIT>()
{
	
}

template<>
void cmGameScene::StateClick<cmGameState::INIT>(int x, int y, int E_BTN)
{
	if (E_BTN != MOUSE_LEFT_BTN)
	{
		return;
	}

	for (int i = 0; i < 4; i++)
	{
		if (avatarBtn[i]->isIn(x, y))
		{
			selectedAvatar = i;
			return;
		}
	}

	if (confirmBtn->isIn(x, y) && GetWindowTextLength(hName) > 0)
	{
		//TODO
		SendLoginSignal();
	}
}

void cmGameScene::SendLoginSignal()
{
	PACKET_LOGIN_INFO loginPac;
	memset(&loginPac, 0, sizeof(loginPac));

	loginPac.header.type = PACKET_TYPE_LOGIN_INFO;
	loginPac.header.size = sizeof(loginPac);

	loginPac.avatar = selectedAvatar;
	GetWindowText(hName, loginPac.name, MAX_NAME_LENGTH);

	SendToServer(&loginPac, loginPac.header.size);
}
#pragma endregion

#pragma region STATE - LOBBY

template<>
void cmGameScene::ProcessPacket<cmGameState::LOBBY>(int type, int size)
{

}

template<>
void cmGameScene::StateDraw<cmGameState::LOBBY>(HDC hdc)
{

}

template<>
void cmGameScene::StateInit<cmGameState::LOBBY>()
{

}

template<>
void cmGameScene::StateRelease<cmGameState::LOBBY>()
{

}

template<>
void cmGameScene::StateUpdate<cmGameState::LOBBY>()
{

}

template<>
void cmGameScene::StateClick<cmGameState::LOBBY>(int x, int y, int E_BTN)
{

}
#pragma endregion

#pragma region STATE - IN GAME
template<>
void cmGameScene::ProcessPacket<cmGameState::IN_GAME>(int type, int size)
{

}

template<>
void cmGameScene::StateDraw<cmGameState::IN_GAME>(HDC hdc)
{

}

template<>
void cmGameScene::StateInit<cmGameState::IN_GAME>()
{

}

template<>
void cmGameScene::StateRelease<cmGameState::IN_GAME>()
{

}

template<>
void cmGameScene::StateUpdate<cmGameState::IN_GAME>()
{

}

template<>
void cmGameScene::StateClick<cmGameState::IN_GAME>(int x, int y, int E_BTN)
{

}
#pragma endregion

void cmGameScene::SendToServer(void* source, int size)
{
	send(g_sock, (const char*)source, size, 0);
}

void cmGameScene::ChangeStateTo(cmGameState nextState)
{
	StateReleaseCaller();
	state = nextState;
	StateInitCaller();
}
