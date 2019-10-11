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
	imgMap[IMG_SKETCH_PLANE] = new Image(L"res/sketch.bmp");
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
	switch (type)
	{

	case PACKET_TYPE_LOBBY_INFO:
	{
		PACKET_LOBBY_INFO lobypac;
		memcpy(&lobypac, readBuf, size);
		lobbyInfo.resize(lobypac.roomNum);
		for (int i = 0; i < lobbyInfo.size(); i++)
		{
			lobbyInfo[i].first = lobypac.playerNum[i];
			lobbyInfo[i].second = lobypac.isPlaying[i];
		}
		ChangeStateTo(cmGameState::LOBBY);
	}
	break;

	}
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
	switch (type)
	{

	case PACKET_TYPE_LOBBY_INFO:
	{
		PACKET_LOBBY_INFO lobypac;
		memcpy(&lobypac, readBuf, size);	
		for (int i = 0; i < lobbyInfo.size(); i++)
		{
			lobbyInfo[i].first = lobypac.playerNum[i];
			lobbyInfo[i].second = lobypac.isPlaying[i];
		}
	}
	break;

	case PACKET_TYPE_LOBBY_PLAYERS:
	{
		PACKET_LOBBY_PLAYERS loppac;
		memcpy(&loppac, readBuf, size);
		gameUserInfoVec.resize(MAX_PLAYERS_IN_ROOM);
		for (int i = 0; i < gameUserInfoVec.size(); i++)
		{
			gameUserInfoVec[i].avatarNum = loppac.avatar[i];
			if (gameUserInfoVec[i].avatarNum == -1)
			{
				continue;
			}
			
			gameUserInfoVec[i].isReady = loppac.isReady[i];
			memcpy_s(gameUserInfoVec[i].name, sizeof(gameUserInfoVec[i].name), loppac.nameArr[i], sizeof(wchar_t)*MAX_NAME_LENGTH);
			ChangeStateTo(cmGameState::IN_GAME);
		}
	}
	break;

	case PACKET_TYPE_LOBBY_CHAT:
	{
		PACKET_LOBBY_CHAT lobchatpac;
		memcpy(&lobchatpac, readBuf, size);
		std::wstring read(lobchatpac.playerName);
		read.append(L" : ");
		for (int i = 0; i < lobchatpac.chatLength; i++)
		{
			read.push_back(lobchatpac.msg[i]);
		}
		chatBuf.push_back(read);
	}
	break;

	}
}

template<>
void cmGameScene::StateDraw<cmGameState::LOBBY>(HDC hdc)
{
	int startPosX = lobbyArrange.startposX;
	int startPosY = lobbyArrange.startposY;
	int width = lobbyArrange.width;
	int height = lobbyArrange.height;
	int rows = lobbyArrange.rows;
	int cols = lobbyArrange.cols;
	int dx = lobbyArrange.dx;
	int dy = lobbyArrange.dy;

	wchar_t charbuf[20];

	int index = 0;
	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++, index++)
		{
			int curX = startPosX + dx * c + width * c;
			int curY = startPosY + dy * r + height * r;
			MoveToEx(hdc, curX, curY, NULL);
			LineTo(hdc, curX + width, curY);
			LineTo(hdc, curX + width, curY + height);
			LineTo(hdc, curX, curY + height);
			LineTo(hdc, curX, curY);

			if (lobbyInfo[index].second)
			{
				wsprintf(charbuf, TEXT("PLAYING NOW"));
			}
			else
			{
				wsprintf(charbuf, TEXT("WAITING - %d"), lobbyInfo[index].first);
			}

			TextOut(hdc, curX, curY, charbuf, lstrlen(charbuf));
		}
	}

	render.SetImg(imgMap[IMG_BTN_REFRESH]);
	render.SetPosition(refreshBtn->xpos, refreshBtn->ypos);
	render.Render(hdc);

	{
		int startX = 1000;
		int startY = 50;
		int dy = 30;
		int index = 0;
		for (auto iter = chatBuf.begin(); iter != chatBuf.end(); iter++, index++)
		{
			TextOut(hdc, startX, startPosY + dy * index, iter->c_str(), iter->length());
		}
	}
}

template<>
void cmGameScene::StateInit<cmGameState::LOBBY>()
{
	hChat = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL, 1000, 550, 300, 30, ProgramCore::instance.getHWND(), (HMENU)idChat, ProgramCore::instance.getGinst(), NULL);
	SendMessage(hChat, EM_LIMITTEXT, (WPARAM)MAX_CHAT_LENGTH, 0);
	chatBuf.clear();
	refreshBtn = new pgBtn(670, 310 - imgMap[IMG_BTN_REFRESH]->bmHeight, imgMap[IMG_BTN_REFRESH]->bmWidth, imgMap[IMG_BTN_REFRESH]->bmHeight);

	lobbyArrange.startposX = 50;
	lobbyArrange.startposY = 50;
	lobbyArrange.width = 100;
	lobbyArrange.height = 120;
	lobbyArrange.rows = 2;
	lobbyArrange.cols = 5;
	lobbyArrange.dx = 20;
	lobbyArrange.dy = 20;
}

template<>
void cmGameScene::StateRelease<cmGameState::LOBBY>()
{
	DestroyWindow(hChat);
	delete refreshBtn;
}

template<>
void cmGameScene::StateUpdate<cmGameState::LOBBY>()
{
	// enter key pressed
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SendChatMsg();
	}
	if (chatBuf.size() > maxChatSize)
	{
		chatBuf.pop_front();
	}
}

template<>
void cmGameScene::StateClick<cmGameState::LOBBY>(int x, int y, int E_BTN)
{
	if (E_BTN != MOUSE_LEFT_BTN)
	{
		return;
	}

	if (refreshBtn->isIn(x, y))
	{
		SendLobbyRefMsg();
		return;
	}

	int startPosX = lobbyArrange.startposX;
	int startPosY = lobbyArrange.startposY;
	int width = lobbyArrange.width;
	int height = lobbyArrange.height;
	int rows = lobbyArrange.rows;
	int cols = lobbyArrange.cols;
	int dx = lobbyArrange.dx;
	int dy = lobbyArrange.dy;

	for (int r = 0; r < rows; r++)
	{
		for (int c = 0; c < cols; c++)
		{
			int curX = startPosX + dx * c + width * c;
			int curY = startPosY + dy * r + height * r;
			int lastX = curX + width;
			int lastY = curY + height;
			if (x >= curX && x <= lastX && y >= curY && y <= lastY)
			{
				// TODO
				SendLobbyIn(cols*r + c);
				return;
			}
		}
	}
}

void cmGameScene::SendChatMsg()
{
	int length = GetWindowTextLength(hChat);
	if (length == 0)
	{
		return;
	}

	PACKET_LOBBY_CHAT lobchatpac;
	lobchatpac.header.type = PACKET_TYPE_LOBBY_CHAT;
	lobchatpac.chatLength = length;
	GetWindowText(hChat, lobchatpac.msg, MAX_CHAT_LENGTH);
	lobchatpac.header.size = sizeof(PACKET_LOBBY_CHAT) - (MAX_CHAT_LENGTH - length) * sizeof(wchar_t);

	SetWindowText(hChat, L"");

	SendToServer(&lobchatpac, lobchatpac.header.size);
}

void cmGameScene::SendLobbyIn(int index)
{
	PACKET_LOBBY_IN lobinpac;
	lobinpac.header.type = PACKET_TYPE_LOBBY_IN;
	lobinpac.header.size = sizeof(PACKET_LOBBY_IN);
	lobinpac.index = index;
	SendToServer(&lobinpac, lobinpac.header.size);
}

void cmGameScene::SendLobbyRefMsg()
{
	PACKET_LOBBY_IN lobinpac;
	lobinpac.header.type = PACKET_TYPE_LOBBY_IN;
	lobinpac.header.size = sizeof(PACKET_LOBBY_IN);
	lobinpac.index = -1;
	SendToServer(&lobinpac, lobinpac.header.size);
}
#pragma endregion

#pragma region STATE - IN GAME
template<>
void cmGameScene::ProcessPacket<cmGameState::IN_GAME>(int type, int size)
{
	switch (type)
	{

	case PACKET_TYPE_LOBBY_CHANGE:
	{
		PACKET_LOBBY_CHANGE lobchanpac;
		memcpy_s(&lobchanpac, sizeof(lobchanpac), readBuf, size);

		GameUserInfo chuser;
		chuser.avatarNum = lobchanpac.avatar;
		chuser.isReady = lobchanpac.rdyState;
		memcpy_s(chuser.name, sizeof(chuser.name), lobchanpac.name, sizeof(lobchanpac.name));
		if (lobchanpac.seatBefore == -1) // new come
		{
			gameUserInfoVec[lobchanpac.seatAfter] = chuser;
			break;
		}		
		gameUserInfoVec[lobchanpac.seatBefore].avatarNum = -1;
		if (lobchanpac.seatAfter != -1) 
		{
			gameUserInfoVec[lobchanpac.seatAfter] = chuser;
		}
		else// out
		{
			break;			
		}
			
	}
	break;

	case PACKET_TYPE_LOBBY_CHAT:
	{
		PACKET_LOBBY_CHAT lobchatpac;
		memcpy(&lobchatpac, readBuf, size);
		std::wstring read(lobchatpac.playerName);
		read.append(L" : ");
		for (int i = 0; i < lobchatpac.chatLength; i++)
		{
			read.push_back(lobchatpac.msg[i]);
		}
		chatBuf.push_back(read);
		if (chatBuf.size() > maxChatSizeInGame)
		{
			chatBuf.pop_front();
		}
	}
	break;

	default:
		break;
	}
}

template<>
void cmGameScene::StateDraw<cmGameState::IN_GAME>(HDC hdc)
{
	render.SetImg(imgMap[IMG_SKETCH_PLANE]);
	render.SetPosition(sketchPlane->xpos, sketchPlane->ypos);
	render.Render(hdc);

	//550, 700
	int chatStartX = 550;
	int chatStartY = 650;
	int chatDy = 30;
	int chatIdx = 0;
	for (auto iter = chatBuf.rbegin(); iter != chatBuf.rend(); iter++, chatIdx++)
	{
		TextOut(hdc, chatStartX, chatStartY - chatDy * chatIdx, iter->c_str(), iter->size());
	}

	int index = 0;
	for (int r = 0; r < gameArrange.rows; r++)
	{
		for (int c = 0; c < gameArrange.cols; c++,index++)
		{
			int avatar = gameUserInfoVec[index].avatarNum;
			if (avatar == -1)
			{
				continue;
			}
			int xpos = gameArrange.getPosX(c);
			int ypos = gameArrange.getPosY(r);
			render.SetImg(imgMap[IMG_AVATAR_1 + avatar]);
			render.SetPosition(xpos,ypos);
			render.Render(hdc);

			TextOut(hdc, xpos, ypos + gameArrange.height, gameUserInfoVec[index].name, wcslen(gameUserInfoVec[index].name));

			if (gameUserInfoVec[index].isReady)
			{
				render.SetImg(imgMap[IMG_AURA_READY]);
				render.Render(hdc);
			}
		}
	}

	render.SetImg(imgMap[IMG_BTN_READY]);
	render.SetPosition(readyBtn->xpos, readyBtn->ypos);
	render.Render(hdc);

	render.SetImg(imgMap[IMG_BTN_EXIT]);
	render.SetPosition(exitBtn->xpos, exitBtn->ypos);
	render.Render(hdc);
}

template<>
void cmGameScene::StateInit<cmGameState::IN_GAME>()
{
	hChat = CreateWindow(TEXT("edit"), NULL, WS_CHILD | WS_BORDER | WS_VISIBLE | ES_AUTOHSCROLL, 550, 700, 300, 30, ProgramCore::instance.getHWND(), (HMENU)idChat, ProgramCore::instance.getGinst(), NULL);
	SendMessage(hChat, EM_LIMITTEXT, (WPARAM)MAX_CHAT_LENGTH, 0);
	chatBuf.clear();

	readyBtn = new pgBtn(50, 700, imgMap[IMG_BTN_READY]->bmWidth, imgMap[IMG_BTN_READY]->bmHeight);
	exitBtn = new pgBtn(300, 700, imgMap[IMG_BTN_EXIT]->bmWidth, imgMap[IMG_BTN_EXIT]->bmHeight);
	sketchPlane = new pgBtn(200,50,imgMap[IMG_SKETCH_PLANE]->bmWidth,imgMap[IMG_SKETCH_PLANE]->bmHeight);

	gameArrange.startposX = 50;
	gameArrange.startposY = 50;
	gameArrange.width = imgMap[IMG_AVATAR_1]->bmWidth;
	gameArrange.height = imgMap[IMG_AVATAR_1]->bmHeight;
	gameArrange.rows = 4;
	gameArrange.cols = 2;
	gameArrange.dx = 1000;
	gameArrange.dy = 100;

}

template<>
void cmGameScene::StateRelease<cmGameState::IN_GAME>()
{
	DestroyWindow(hChat);

	delete readyBtn;
	delete exitBtn;
	delete sketchPlane;
}

template<>
void cmGameScene::StateUpdate<cmGameState::IN_GAME>()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		SendChatMsg();
	}
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
