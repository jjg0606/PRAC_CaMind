#pragma once
#include <vector>
#include <string>
#include <WinSock2.h>
#include "PACKET.h"
#define BUFSIZE 512

enum class cmUserState
{
	DEFAULT,
	LOBBY,
	GAME,
	STATE_END,
};

class cmGameUser
{
#pragma region IOVariables
	SOCKET sock;
	std::string client_addr;
	int client_port;
	
	char readBuf[BUFSIZE + 1];
	char sendBuf[BUFSIZE + 1];
	int recvBytes;
	int sendBytes;
	static const int DefaultSleepTime = 10;
#pragma endregion

#pragma region IO Util Function
	bool SendToUser();
	void ReadSet();
	void ProcessByteStream();
	bool CopyToSendbuf(void* source, int size);
#pragma endregion

	// Msg Handler
	template<cmUserState S>
	void HandlePacket(int type,int size);
	void HandlePacketCall(int type, int size);

#pragma region BasicVariables
	cmUserState state = cmUserState::DEFAULT;
	wchar_t name[MAX_NAME_LENGTH];
	int avatarNum = DefaultAvatarNum;
#pragma endregion
	
#pragma region StateVariables
	struct LobbyVariables
	{
		int roomNum = NotInRoom;
	};
	LobbyVariables lobbyVar;
	
#pragma endregion

#pragma region MessageHandleFunctions
	// Called in Default
	void HandleLogInMsg(int size);
	// Called in Lobby
	void HandleLobbyInMsg(int size);
	void HandleLobbyChatMsg(int size);
	// Called in Game
	void HandleInGameChatMsg(int size);
	void HandleGameRoomExit(int size);
	void HandleGameRdySignal();
	void HandleGamePointsSignal(int size);
	void HandleGamePointClearSignal(int size);
#pragma endregion

public:
	static const int DefaultAvatarNum = -1;
	static const int NotInRoom = -1;
	WSAOVERLAPPED overlapped;
	WSABUF wsabuf;
	explicit cmGameUser(SOCKET sock,std::string client_addr,int client_port);
	~cmGameUser();
	void Process(DWORD cbTransferred);
	void InitOverlapped();

	int GetAvatar();
	void GetNameCopy(wchar_t* dest);
	bool SendPacket(void* packet, int size);
};

#pragma region TEMPLATE SPECIALIZED
template<>
void cmGameUser::HandlePacket<cmUserState::DEFAULT>(int type, int size);

template<>
void cmGameUser::HandlePacket<cmUserState::LOBBY>(int type, int size);

template<>
void cmGameUser::HandlePacket<cmUserState::GAME>(int type, int size);
#pragma endregion


