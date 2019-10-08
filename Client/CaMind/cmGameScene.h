#pragma once
#include "Image.h"
#include <map>
#include "Scene.h"
#include "DrawObject.h"
#define BUFSIZE 512

enum class cmGameState
{
	INIT,
	LOBBY,
	IN_GAME,
};

class cmGameScene
	: virtual public Scene
{
	std::map<int, Image*> imgMap;
	std::vector<std::pair<int, int>> posList;
	DrawObject dro;
	template<cmGameState S>
	void ProcessPacket(int type, int size) {};

	void ProcessPacketCaller(int type,int size);
	void ProcessPacketByteStream();
	int bufIdx = 0;
	char readBuf[BUFSIZE];

	cmGameState state = cmGameState::INIT;
	void SendToServer(void* source, int size);
	void LoadImg();
public:
	void DrawScreen(HDC hdc) override;
	void OnInput(WPARAM wParam) override;
	void Update() override;
	void Release() override;
	void Init() override;
	void OnMouseClick(int x, int y, int E_MOUSE_BTN) override;
	void OnMouseMove() override;
	void OnSocketEvent(UINT iMessage, WPARAM wParam, LPARAM lParam) override;
	~cmGameScene();
};

template<>
void cmGameScene::ProcessPacket<cmGameState::INIT>(int type, int size);

template<>
void cmGameScene::ProcessPacket<cmGameState::LOBBY>(int type, int size);

template<>
void cmGameScene::ProcessPacket<cmGameState::IN_GAME>(int type, int size);