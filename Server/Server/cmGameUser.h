#pragma once
#include <vector>
#include <string>
#include <WinSock2.h>
#include "PACKET.h"
#define BUFSIZE 512

enum class cmUserState
{
	DEFAULT,
	READMSG,
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
	void Read(DWORD cbTransferred);
	void Send();
	void ReadSet();
	bool CopyToSendbuf(void* source, int size);
	cmUserState state = cmUserState::DEFAULT;

	void UpateCall(int type,int size);
	void ProcessByteStream();

	template<cmUserState S>
	void Update(int type,int size);
public:
	WSAOVERLAPPED overlapped;
	WSABUF wsabuf;
	explicit cmGameUser(SOCKET sock,std::string client_addr,int client_port);
	void Process(DWORD cbTransferred);
	void InitOverlapped();
};

template<>
void cmGameUser::Update<cmUserState::DEFAULT>(int type,int size);

template<>
void cmGameUser::Update<cmUserState::READMSG>(int type,int size);