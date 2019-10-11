#pragma once
#include <vector>
#include <string>
#include <WinSock2.h>
#include <mutex>
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
	SOCKET sock;
	std::string client_addr;
	int client_port;
	
	char readBuf[BUFSIZE + 1];
	char sendBuf[BUFSIZE + 1];
	int recvBytes;
	int sendBytes;
	bool isReading = true;
	wchar_t name[MAX_NAME_LENGTH];
	int avatarNum = -1;

#pragma region IO Util Function
	void Read(DWORD cbTransferred);
	void Send();
	void ReadSet();
	void ProcessByteStream();
	bool CopyToSendbuf(void* source, int size);
#pragma endregion

	cmUserState state = cmUserState::DEFAULT;
	
	template<cmUserState S>
	void Update(int type,int size);
	void UpateCall(int type, int size);
	//DEFAULT
	//LOBBY
	int roomNum = -1;

public:
	std::mutex userMtx;
	WSAOVERLAPPED overlapped;
	WSABUF wsabuf;
	explicit cmGameUser(SOCKET sock,std::string client_addr,int client_port);
	~cmGameUser();
	void Process(DWORD cbTransferred);
	void InitOverlapped();

	int getAvatar();
	void getNameCopy(wchar_t* dest);
	void SendPacket(void* packet, int size);
};

#pragma region TEMPLATE SPECIALIZED
template<>
void cmGameUser::Update<cmUserState::DEFAULT>(int type, int size);

template<>
void cmGameUser::Update<cmUserState::LOBBY>(int type, int size);

template<>
void cmGameUser::Update<cmUserState::GAME>(int type, int size);
#pragma endregion


