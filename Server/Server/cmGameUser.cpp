#include "cmGameUser.h"
#include "cmRoomMgr.h"
#include "cmLobbyMgr.h"

cmGameUser::cmGameUser(SOCKET sock, std::string client_addr, int client_port)
	: sock(sock), client_addr(client_addr), client_port(client_port)
{
	ZeroMemory(&this->overlapped, sizeof(this->overlapped));
	this->recvBytes = this->sendBytes = 0;
	this->wsabuf.buf = this->readBuf;
	this->wsabuf.len = BUFSIZE;

	printf("[TCP server] client connected : ip = %s, port = %d\n", client_addr.c_str(), client_port);
}

cmGameUser::~cmGameUser()
{
	cmLobbyMgr::instance.GetOut(this);
	if (lobbyVar.roomNum != NotInRoom)
	{
		cmRoomMgr::instance[lobbyVar.roomNum]->RequestRoomOut(this);
	}
}

void cmGameUser::Process(DWORD cbTransferred)
{
	// receive
	recvBytes += cbTransferred;
	ProcessByteStream();
	ReadSet();
}

#pragma region IO Util Function
bool cmGameUser::SendToUser()
{
	InitOverlapped();
	wsabuf.buf = sendBuf;
	wsabuf.len = sendBytes;

	int retval = send(sock, sendBuf, sendBytes, 0);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf("WSASend()");
		}
		return false;
	}	
	printf("[TCP/%s:%d] <SEND> size : %d\n", client_addr.c_str(), client_port, sendBytes);
	sendBytes = 0;
	Sleep(DefaultSleepTime);
	return true;
}

void cmGameUser::ReadSet()
{
	InitOverlapped();
	wsabuf.buf = readBuf + recvBytes;
	wsabuf.len = BUFSIZE - recvBytes;

	DWORD recvbytes;
	DWORD flags = 0;
	int retval = WSARecv(sock, &wsabuf, 1, &recvbytes, &flags, &overlapped, NULL);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf("WSARecv()");
		}
		return;
	}
}

void cmGameUser::ProcessByteStream()
{
	bool isUpdateCalled = false;
	while (true)
	{
		if (recvBytes < sizeof(PACKET_HEADER))
		{
			break;
		}

		PACKET_HEADER header;
		memcpy(&header, readBuf, sizeof(header));

		if (recvBytes < header.size)
		{
			break;
		}
		isUpdateCalled = true;
		printf("[TCP/%s:%d] <READ> type : %d, size : %d\n", client_addr.c_str(), client_port, header.type, header.size);
		HandlePacketCall(header.type,header.size);

		for (int i = header.size; i < recvBytes ; i++)
		{
			readBuf[i - header.size] = readBuf[i];
		}

		recvBytes -= header.size;
	}
}

bool cmGameUser::CopyToSendbuf(void* source, int size)
{
	if (sendBytes + size >= BUFSIZE)
	{
		return false;
	}
	memcpy(sendBuf + sendBytes, source, size);
	sendBytes += size;
	return true;
}

void cmGameUser::InitOverlapped()
{
	ZeroMemory(&this->overlapped, sizeof(this->overlapped));
}

bool cmGameUser::SendPacket(void* packet, int size)
{
	if (!CopyToSendbuf(packet, size))
	{
		return false;
	}

	return SendToUser();
	
}
#pragma endregion

#pragma region Getter

int cmGameUser::GetAvatar()
{
	return avatarNum;
}

void cmGameUser::GetNameCopy(wchar_t* dest)
{
	for (int i = 0; i < MAX_NAME_LENGTH; i++)
	{
		dest[i] = this->name[i];
	}
}

#pragma endregion

#pragma region Call Function
void cmGameUser::HandlePacketCall(int type, int size)
{
	switch (state)
	{
	case cmUserState::DEFAULT:
		HandlePacket<cmUserState::DEFAULT>(type, size);
		break;
	case cmUserState::LOBBY:
		HandlePacket<cmUserState::LOBBY>(type, size);
		break;
	case cmUserState::GAME:
		HandlePacket<cmUserState::GAME>(type, size);
		break;
	}
}

#pragma endregion

template<cmUserState S>
void cmGameUser::HandlePacket(int type,int size)
{
	// this function only be called at wrong state
	printf("[TCP SERVER] %s : %d , error update call unexpected state\n", client_addr.c_str(), client_port);
}

#pragma region Default State
template<>
void cmGameUser::HandlePacket<cmUserState::DEFAULT>(int type, int size)
{
	switch (type)
	{

	case PACKET_TYPE_LOGIN_INFO:
		if (avatarNum == DefaultAvatarNum)
		{
			HandleLogInMsg(size);
		}
		break;

	default:
		break;
	}
}

void cmGameUser::HandleLogInMsg(int size)
{
	PACKET_LOGIN_INFO loginPac;
	memcpy(&loginPac, readBuf, size);
	for (int i = 0; i < MAX_NAME_LENGTH; i++)
	{
		name[i] = loginPac.name[i];
	}
	avatarNum = loginPac.avatar;
	cmRoomMgr::instance.GetAllRoomInfo(this);
	cmLobbyMgr::instance.GetIn(this);
	state = cmUserState::LOBBY;
}

#pragma endregion

#pragma region Lobby State
template<>
void cmGameUser::HandlePacket<cmUserState::LOBBY>(int type, int size)
{
	switch (type)
	{

	case PACKET_TYPE_LOBBY_IN:	
	HandleLobbyInMsg(size);	
	break;

	case PACKET_TYPE_LOBBY_CHAT:
	HandleLobbyChatMsg(size);
	break;

	default:
		break;
	}
}

void cmGameUser::HandleLobbyInMsg(int size)
{
	PACKET_LOBBY_IN lobinpac;
	memcpy(&lobinpac, readBuf, size);
	if (lobinpac.index < 0 || lobinpac.index >= MAX_LOBBY)
	{
		// process refresh signal
		cmRoomMgr::instance.GetAllRoomInfo(this);
	}
	else
	{
		if (cmRoomMgr::instance[lobinpac.index]->RequestRoomIn(this))
		{
			cmLobbyMgr::instance.GetOut(this);
			state = cmUserState::GAME;
			lobbyVar.roomNum = lobinpac.index;
		}
	}
}

void cmGameUser::HandleLobbyChatMsg(int size)
{
	PACKET_LOBBY_CHAT lobchatpac;
	memcpy(&lobchatpac, readBuf, size);
	for (int i = 0; i < MAX_NAME_LENGTH; i++)
	{
		lobchatpac.playerName[i] = name[i];
	}
	cmLobbyMgr::instance.BroadCast(&lobchatpac, size);
}
#pragma endregion

#pragma region Game State
template<>
void cmGameUser::HandlePacket<cmUserState::GAME>(int type, int size)
{
	switch (type)
	{

	case PACKET_TYPE_LOBBY_CHAT:
	HandleInGameChatMsg(size);
	break;

	case PACKET_TYPE_LOBBY_IN:
	HandleGameRoomExit(size);
	break;

	case PACKET_TYPE_GAME_READY:
	HandleGameRdySignal();
	break;

	case PACKET_TYPE_GAME_POINTS:
	HandleGamePointsSignal(size);
	break;

	case PACKET_TYPE_SYSTEM:
	{
		PACKET_SYSTEM syspac;
		memcpy(&syspac, readBuf, size);
		switch (syspac.system_msg)
		{
		case SYSTEM_MSG_GAME_POINT_CLEAR:
		HandleGamePointClearSignal(size);
		break;
		}
	}
	break;
	}
}

void cmGameUser::HandleInGameChatMsg(int size)
{
	PACKET_LOBBY_CHAT lobchatpac;
	memcpy(&lobchatpac, readBuf, size);
	for (int i = 0; i < MAX_NAME_LENGTH; i++)
	{
		lobchatpac.playerName[i] = name[i];
	}

	std::wstring ans;
	for (int i = 0; i < lobchatpac.chatLength; i++)
	{
		ans.push_back(lobchatpac.msg[i]);
	}
	cmRoomMgr::instance[lobbyVar.roomNum]->ChkAnswer(this,ans);
	cmRoomMgr::instance[lobbyVar.roomNum]->BroadCastChatMsg(this,&lobchatpac, lobchatpac.header.size);
}

void cmGameUser::HandleGameRoomExit(int size)
{
	PACKET_LOBBY_IN lobinPac;
	memcpy_s(&lobinPac, sizeof(lobinPac), readBuf, size);
	if (lobinPac.index != -1)
	{
		return;
	}

	cmRoomMgr::instance[lobbyVar.roomNum]->RequestRoomOut(this);

	state = cmUserState::LOBBY;

	std::wstring msg;

	cmLobbyMgr::instance.GetIn(this);
	cmRoomMgr::instance.GetAllRoomInfo(this);
}

void cmGameUser::HandleGameRdySignal()
{
	cmGameRoom* curRoom = cmRoomMgr::instance[lobbyVar.roomNum];
	if (curRoom == nullptr)
	{
		return;
	}
	curRoom->ToggleReady(this);
}

void cmGameUser::HandleGamePointsSignal(int size)
{
	cmRoomMgr::instance[lobbyVar.roomNum]->PointsPacketSync(this, readBuf, size);
}

void cmGameUser::HandleGamePointClearSignal(int size)
{
	cmRoomMgr::instance[lobbyVar.roomNum]->PointClearOrder(this,readBuf,size);
}

#pragma endregion