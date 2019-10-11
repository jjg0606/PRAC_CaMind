#pragma comment(lib,"ws2_32")
#include <WinSock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <string>
#include "cmGameUser.h"
#include "cmUserMgr.h"

#define SERVERPORT 8989
#define BUFSIZE 512


DWORD WINAPI WorkThread(LPVOID arg);

void err_quit(const char* msg);
void err_display(const char* msg);

int main()
{
	int retval;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return 1;
	}
	// create completion port
	HANDLE hcp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (hcp == NULL)
	{
		return 1;
	}

	// check cpu num
	SYSTEM_INFO si;
	GetSystemInfo(&si);

	HANDLE hThread;
	for (int i = 0; i < (int)si.dwNumberOfProcessors * 2; i++)
	{
		hThread = CreateThread(NULL, 0, WorkThread, hcp, 0, NULL);
		if (hThread == NULL)
		{
			return 1;
		}
		CloseHandle(hThread);
	}

	// socket
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET)
	{
		err_quit("socket()");
	}
	//bind
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		err_quit("bind()");
	}
	//listen
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		err_quit("listen()");
	}

	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen;
	DWORD recvbytes, flags;

	while (true)
	{
		//accpet
		addrlen = sizeof(clientaddr);
		client_sock = accept(listen_sock, (SOCKADDR*)&clientaddr, &addrlen);
		if (client_sock == INVALID_SOCKET)
		{
			err_display("accept()");
			break;
		}
		CreateIoCompletionPort((HANDLE)client_sock, hcp, client_sock, 0);
		cmUserMgr::instance.Insert(client_sock, inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
		cmGameUser* curUser = cmUserMgr::instance[client_sock];
		if (curUser == nullptr)
		{
			break;
		}

		/*SOCKETINFO* ptr = new SOCKETINFO;
		if (ptr == NULL)
		{
			break;
		}*/
		
		/*ZeroMemory(&ptr->overlapped, sizeof(ptr->overlapped));
		ptr->sock = client_sock;
		ptr->recvbytes = ptr->sendbytes = 0;
		ptr->wsabuf.buf = ptr->buf;
		ptr->wsabuf.len = BUFSIZE;*/

		// start async io
		flags = 0;
		retval = WSARecv(client_sock, &curUser->wsabuf, 1, &recvbytes, &flags, &curUser->overlapped, NULL);
		if (retval == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
			{
				err_display("WSARecv()");
			}
			continue;
		}
	}



	WSACleanup();
	return 0;
}


void err_quit(const char* msg)
{
	int len = strlen(msg);
	wchar_t buf[BUFSIZE];
	mbstowcs(buf, msg, len);

	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL
	);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, buf, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL
	);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

DWORD WINAPI WorkThread(LPVOID arg)
{
	int retval;
	HANDLE hcp = (HANDLE)arg;
	while (true)
	{
		DWORD cbTransferred;
		SOCKET client_sock;
		WSAOVERLAPPED overlap;
		//SOCKETINFO* ptr;
		retval = GetQueuedCompletionStatus(hcp, &cbTransferred, (LPDWORD)&client_sock, (LPOVERLAPPED*)&overlap, INFINITE);

		// display connected client
		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(client_sock, (SOCKADDR*)&clientaddr, &addrlen);

		std::string ipaddr = inet_ntoa(clientaddr.sin_addr);
		int port = ntohs(clientaddr.sin_port);

		cmGameUser* curUser = cmUserMgr::instance[client_sock];
		if (curUser == nullptr)
		{
			err_display("User is null ptr\n");
			closesocket(client_sock);
			continue;
		}
		//
		//curUser->userMtx.lock();
		// TODO
		if (retval == 0 || cbTransferred == 0)
		{
			if (retval == 0)
			{
				DWORD temp1, temp2;
				WSAGetOverlappedResult(client_sock, &curUser->overlapped, &temp1, FALSE, &temp2);
				err_display("WSAGetOverlappedResult()");
			}
			closesocket(client_sock);
			printf("[TCP 서버] 클라이언트 종료 : IP 주소 = %s, 포트번호 = %d\n", ipaddr.c_str(), port);
			cmUserMgr::instance.Delete(client_sock);
			continue;
		}

		curUser->Process(cbTransferred);
		//
		//curUser->userMtx.unlock();
	}

	return 0;
}
