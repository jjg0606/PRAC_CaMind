#pragma comment(lib, "msimg32.lib")
#pragma comment(lib,"ws2_32")
#include "SettingLoader.h"
#include <WinSock2.h>
#include "WndBase.h"
#include "ProgramCore.h"
#include "EnumMouse.h"

HINSTANCE g_hinst;
SOCKET g_sock;

#define WM_SOCKET (WM_USER+1)
#define PORT 8989

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	// window handle
	HWND hWnd;
	MSG Message;
	WNDCLASS WndClass;
	g_hinst = hInstance;
	ProgramCore::instance.setGinst(hInstance);
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hInstance = hInstance;
	WndClass.lpfnWndProc = WndProc;
	WndClass.lpszClassName = lpszClass;
	WndClass.lpszMenuName = NULL;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&WndClass);

	hWnd = CreateWindow(lpszClass, lpszClass, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, (HMENU)NULL, hInstance, NULL);

	ShowWindow(hWnd, nCmdShow);

	if (!SettingLoader::isLoaded())
	{
		return -1;
	}

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		return -1;
	}



	g_sock = socket(SettingLoader::getIPType(), SettingLoader::getProtocol(), 0);
	if (g_sock == INVALID_SOCKET)
	{
		return -1;
	}

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = SettingLoader::getIPType();
	serveraddr.sin_port = htons(SettingLoader::getPort());	
	serveraddr.sin_addr.s_addr = inet_addr(SettingLoader::instance["serverip"].c_str());
//	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	int retval = connect(g_sock, (sockaddr*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		return -1;
	}

	retval = WSAAsyncSelect(g_sock, hWnd, WM_SOCKET, FD_READ | FD_CLOSE);
	if (retval == SOCKET_ERROR)
	{
		return -1;
	}


	while (GetMessage(&Message, NULL, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}


	closesocket(g_sock);
	WSACleanup();
	return (int)Message.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;
	static int mouseXpos;
	static int mouseYpos;
	ProgramCore::instance.SetHWND(hWnd);
	switch (iMessage)
	{
	case WM_SOCKET:
		ProgramCore::instance.OnSocketEvent(iMessage, wParam, lParam);
		return 0;
	case WM_LBUTTONUP:
		ProgramCore::instance.SetMouseState(MOUSE_LEFT_BTN, MOUSE_BTN_UP);
		return 0;
	case WM_RBUTTONUP:
		ProgramCore::instance.SetMouseState(MOUSE_RIGHT_BTN, MOUSE_BTN_UP);
		return 0;
	case WM_LBUTTONDOWN:
		mouseXpos = LOWORD(lParam);
		mouseYpos = HIWORD(lParam);
		ProgramCore::instance.SetMouseMove(mouseXpos, mouseYpos);
		ProgramCore::instance.SetMouseState(MOUSE_LEFT_BTN, MOUSE_BTN_DOWN);
		ProgramCore::instance.SetMouseClick(mouseXpos, mouseYpos, MOUSE_LEFT_BTN);
		return 0;
	case WM_RBUTTONDOWN:
		mouseXpos = LOWORD(lParam);
		mouseYpos = HIWORD(lParam);
		ProgramCore::instance.SetMouseMove(mouseXpos, mouseYpos);
		ProgramCore::instance.SetMouseState(MOUSE_RIGHT_BTN, MOUSE_BTN_DOWN);
		ProgramCore::instance.SetMouseClick(mouseXpos, mouseYpos, MOUSE_RIGHT_BTN);
		return 0;
	case WM_MOUSEMOVE:
		mouseXpos = LOWORD(lParam);
		mouseYpos = HIWORD(lParam);
		ProgramCore::instance.SetMouseMove(mouseXpos, mouseYpos);
		return 0;
	case WM_KEYDOWN:
		ProgramCore::instance.SetKeyDown(wParam);
		return 0;
	case WM_KEYUP:
		ProgramCore::instance.SetKeyUp(wParam);
		return 0;
	case WM_CHAR:
		ProgramCore::instance.SetInput(wParam);
		return 0;
	case WM_TIMER:
		ProgramCore::instance.Update();
		return 0;
	case WM_CREATE:
		ProgramCore::instance.SetHWND(hWnd);
		ProgramCore::instance.Init();
		ProgramCore::instance.Main();
		return 0;
	case WM_DESTROY:
		closesocket(g_sock);
		ProgramCore::instance.Release();
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		ProgramCore::instance.DrawScreen(hdc);
		EndPaint(hWnd, &ps);
		return 0;
	case WM_COMMAND:
		ProgramCore::instance.SetMenuMessage(wParam);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}