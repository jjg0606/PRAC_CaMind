#pragma once
#include "Scene.h"
#include "Vector2D.h"
#include <chrono>

class ProgramCore
{
private:
	ProgramCore();
	Scene* curScene;
	Scene* nextScene;
	unsigned int hTimer;
	HWND hWnd;
	HDC backBuffer;
	HDC memDC;
	RECT winRect;
	HBITMAP backbuffermap;
	HBITMAP oldBitmap;
	HINSTANCE g_inst;
	void(ProgramCore::*UpdateLast)();
	void DoNothing();
	void ChangeSceneU();
	int cntInputKey;
	bool* inputArr;
	bool mouseInputArr[2] = { false,false };
	Vector2D<int> mousePos{ 0,0 };
	std::chrono::time_point<std::chrono::system_clock> beforeTime;
	std::chrono::duration<float> deltaTime;
public:
	static ProgramCore instance;
	// called at winmain
	void OnSocketEvent(UINT iMessage, WPARAM wParam, LPARAM lParam);
	void setGinst(HINSTANCE _g_inst);
	void DrawScreen(HDC hdc);
	void SetInput(WPARAM wParam);
	void SetKeyDown(WPARAM wParam);
	void SetKeyUp(WPARAM wParam);
	void SetMouseClick(int x, int y, int E_MOUSE_BTN);
	void SetMouseMove(int x, int y);
	void Main();
	void Init();
	void Release();
	void Update();
	void SetHWND(HWND hWnd);
	void SetMenuMessage(WPARAM wParam);
	void SetMouseState(int Btn, int State);
	// called from object
	HDC getBackBuffer();
	HINSTANCE getGinst();
	void ChangeScene(Scene* scene);
	HDC getMemDC();
	bool GetKey(WPARAM keyName);
	bool GetMouseKey(int E_Mouse_Btn);
	Vector2D<int>& getMousePos();
	void SetUpdateIntersec(int mtime);
	void DeleteTimer();
	HWND getHWND();
	std::chrono::duration<float> getDeltaTime();
	void Invalidate();
	RECT& getWinRect();
	Scene* getCurScene();
};
