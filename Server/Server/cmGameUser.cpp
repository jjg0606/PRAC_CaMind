#include "cmGameUser.h"

cmGameUser::cmGameUser(SOCKET sock, std::string client_addr, int client_port)
	: sock(sock), client_addr(client_addr), client_port(client_port)
{
	ZeroMemory(&this->overlapped, sizeof(this->overlapped));
	this->recvBytes = this->sendBytes = 0;
	this->wsabuf.buf = this->readBuf;
	this->wsabuf.len = BUFSIZE;

	printf("[TCP server] client connected : ip = %s, port = %d\n", client_addr.c_str(), client_port);
}

void cmGameUser::Read(DWORD cbTransferred)
{
	int readBefore = recvBytes;
	recvBytes += cbTransferred;
	readBuf[recvBytes] = '\0';
	printf("[TCP/%s:%d] %s\n", client_addr.c_str(), client_port, readBuf + readBefore);
}

void cmGameUser::ProcessByteStream()
{
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

		UpateCall(header.type,header.size);

		for (int i = header.size; i < recvBytes ; i++)
		{
			readBuf[i - header.size] = readBuf[i];
		}

		recvBytes -= header.size;
	}

}

void cmGameUser::Send()
{
	InitOverlapped();
	wsabuf.buf = sendBuf;
	wsabuf.len = sendBytes;

	DWORD sendbytes;
	int retval = WSASend(sock, &wsabuf, 1, &sendbytes, 0, &overlapped, NULL);
	if (retval == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSA_IO_PENDING)
		{
			printf("WSASend()");
		}
		return;
	}	
}

void cmGameUser::Process(DWORD cbTransferred)
{
	// receive
	if (isReading)
	{
		Read(cbTransferred);
		ProcessByteStream();
	}
	else
	{
		sendBytes = 0;
		ReadSet();
	}
	
}

void cmGameUser::InitOverlapped()
{
	ZeroMemory(&this->overlapped, sizeof(this->overlapped));
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

template<cmUserState S>
void cmGameUser::Update(int type,int size)
{
	printf("[TCP SERVER] %s : %d , error update call unexpected state\n", client_addr.c_str(), client_port);
}

void cmGameUser::UpateCall(int type,int size)
{
	switch (state)
	{
	case cmUserState::DEFAULT:
		Update<cmUserState::DEFAULT>(type,size);
		break;
	case cmUserState::READMSG:
		Update<cmUserState::READMSG>(type,size);
		break;
	}
}

template<>
void cmGameUser::Update<cmUserState::DEFAULT>(int type,int size)
{
	isReading = false;
	CopyToSendbuf(readBuf, recvBytes);
	sendBytes = recvBytes;
	recvBytes = 0;
	Send();
}

template<>
void cmGameUser::Update<cmUserState::READMSG>(int type,int size)
{
	
}