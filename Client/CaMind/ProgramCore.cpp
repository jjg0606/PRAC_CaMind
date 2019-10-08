#include "ProgramCore.h"

using namespace std;
ProgramCore ProgramCore::instance;

ProgramCore::ProgramCore()
{
	curScene = nullptr;
	hTimer = 0;
	cntInputKey = 0;
	UpdateLast = &ProgramCore::DoNothing;
	beforeTime = chrono::system_clock::now();
	int arrSize = 1 + 1 << 15;
	inputArr = new bool[arrSize];
	for (int i = 0; i < arrSize; i++)
	{
		inputArr[i] = false;
	}
}

void ProgramCore::Init()
{
	HDC gdc = GetDC(hWnd);
	GetClientRect(hWnd, &winRect);
	backBuffer = CreateCompatibleDC(gdc);
	backbuffermap = CreateCompatibleBitmap(gdc, winRect.right, winRect.bottom);
	memDC = CreateCompatibleDC(backBuffer);
	oldBitmap = (HBITMAP)SelectObject(backBuffer, backbuffermap);
	ReleaseDC(hWnd, gdc);
}

void ProgramCore::Release()
{
	if (curScene != nullptr)
	{
		curScene->Release();
		delete curScene;
	}
	if (this->hTimer != 0)
	{
		KillTimer(hWnd, hTimer);
	}
	delete[] inputArr;
	SelectObject(backBuffer, oldBitmap);
	DeleteObject(backbuffermap);
	DeleteDC(memDC);
	DeleteDC(backBuffer);
}

void ProgramCore::ChangeScene(Scene* scene)
{
	this->nextScene = scene;
	this->UpdateLast = &ProgramCore::ChangeSceneU;
	SetUpdateIntersec(1);
}

void ProgramCore::DrawScreen(HDC hdc)
{
	if (curScene == nullptr)
	{
		return;
	}
	GetClientRect(hWnd, &winRect);
	curScene->DrawScreen(backBuffer);
	//SelectObject(backBuffer, backbuffermap);
	BitBlt(hdc, 0, 0, winRect.right, winRect.bottom, backBuffer, 0, 0, SRCCOPY);
}

void ProgramCore::SetInput(WPARAM wParam)
{
	if (curScene == nullptr)
	{
		return;
	}
	curScene->OnInput(wParam);
}

bool ProgramCore::GetKey(WPARAM keyName)
{
	if (keyName == NULL)
	{
		return cntInputKey == 0;
	}
	return inputArr[keyName];
}

void ProgramCore::SetKeyDown(WPARAM wParam)
{
	//inputChar = LOWORD(wParam);
	if (!inputArr[LOWORD(wParam)])
	{
		inputArr[LOWORD(wParam)] = true;
		cntInputKey++;
	}
}

void ProgramCore::SetKeyUp(WPARAM wParam)
{
	if (inputArr[LOWORD(wParam)])
	{
		inputArr[LOWORD(wParam)] = false;
		cntInputKey--;
	}
}

void ProgramCore::Update()
{
	auto ctime = chrono::system_clock::now();
	deltaTime = ctime - beforeTime;
	beforeTime = ctime;

	if (curScene != nullptr)
	{
		curScene->Update();
	}

	(this->*UpdateLast)();
}

void ProgramCore::SetMouseClick(int x, int y, int E_MOUSE_BTN)
{
	if (curScene == nullptr)
	{
		return;
	}
	curScene->OnMouseClick(x, y, E_MOUSE_BTN);
}

void ProgramCore::SetMouseMove(int x, int y)
{
	mousePos.x = x;
	mousePos.y = y;

	if (curScene != nullptr)
	{
		curScene->OnMouseMove();
	}
}

void ProgramCore::SetMenuMessage(WPARAM wParam)
{
	if (curScene == nullptr)
	{
		return;
	}

	curScene->OnMenuMessage(wParam);
}

void ProgramCore::SetUpdateIntersec(int mtime)
{
	if (this->hTimer != 0)
	{
		KillTimer(hWnd, hTimer);
	}
	SetTimer(hWnd, ++hTimer, mtime, NULL);
}

void ProgramCore::DeleteTimer()
{
	if (this->hTimer != 0)
	{
		KillTimer(hWnd, hTimer);
	}
	this->hTimer = 0;
}

void ProgramCore::DoNothing()
{
	// Nothing
}

void ProgramCore::ChangeSceneU()
{
	// change
	if (curScene != nullptr)
	{
		curScene->Release();
		delete curScene;
	}
	DeleteTimer();
	curScene = nextScene;
	nextScene = nullptr;
	UpdateLast = &ProgramCore::DoNothing;

	curScene->Init();
}

void ProgramCore::SetHWND(HWND hWnd)
{
	this->hWnd = hWnd;
}

std::chrono::duration<float> ProgramCore::getDeltaTime()
{
	return this->deltaTime;
}


void ProgramCore::setGinst(HINSTANCE _g_inst)
{
	this->g_inst = _g_inst;
}

HINSTANCE ProgramCore::getGinst()
{
	return this->g_inst;
}

HDC ProgramCore::getMemDC()
{
	return memDC;
}

HWND ProgramCore::getHWND()
{
	return hWnd;
}

void ProgramCore::Invalidate()
{
	InvalidateRect(hWnd, NULL, FALSE);
}

Vector2D<int>& ProgramCore::getMousePos()
{
	return mousePos;
}

RECT& ProgramCore::getWinRect()
{
	return winRect;
}

Scene* ProgramCore::getCurScene()
{
	return curScene;
}

void ProgramCore::SetMouseState(int Btn, int State)
{
	mouseInputArr[Btn] = State;
}

bool ProgramCore::GetMouseKey(int E_Mouse_Btn)
{
	return mouseInputArr[E_Mouse_Btn];
}

void ProgramCore::OnSocketEvent(UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	if (curScene == nullptr)
	{
		return;
	}

	curScene->OnSocketEvent(iMessage, wParam, lParam);
}

HDC ProgramCore::getBackBuffer()
{
	return backBuffer;
}