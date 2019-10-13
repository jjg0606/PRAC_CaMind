#pragma once
#include <map>
#include <list>
#include "Image.h"
#include "Scene.h"
#include "DrawObject.h"
#include "ImageRenderer.h"
#include "pgBtn.h"
#include "PACKET.h"
#include "cmGameState.h"
#define BUFSIZE 512

typedef struct
{
	int avatarNum;
	bool isReady;
	wchar_t name[MAX_NAME_LENGTH];
}
GameUserInfo;

typedef struct
{
	int startposX;
	int startposY;
	int dx;
	int dy;
	int width;
	int height;
	int rows;
	int cols;

	int getPosX(int col)
	{
		return startposX + col * (dx + width);
	}

	int getPosY(int row)
	{
		return startposY + row * (dy + height);
	}
}
TableArrange;


class cmGameScene
	: virtual public Scene
{
	std::map<int, Image*> imgMap;
	ImageRenderer render;

	std::vector<std::pair<int, int>> posList;
	DrawObject dro;
	
	void ProcessPacketByteStream();
	int bufIdx = 0;
	char readBuf[BUFSIZE];

	cmGameState state = cmGameState::INIT;
	void ChangeStateTo(cmGameState nextState);

	void SendToServer(void* source, int size);
	void LoadImg();
	void SetStateCallers();

#pragma region State Functions
	template<cmGameState S>
	void ProcessPacket(int type, int size) {};
	void(cmGameScene::*ProcessPacketCaller)(int type,int size);

	template<cmGameState S>
	void StateDraw(HDC hdc) {};	
	void(cmGameScene::*StateDrawCaller)(HDC hdc);

	template<cmGameState S>
	void StateInit() {};	
	void(cmGameScene::* StateInitCaller)();
	

	template<cmGameState S>
	void StateRelease() {};
	void(cmGameScene::* StateReleaseCaller)();

	template<cmGameState S>
	void StateUpdate() {};
	void(cmGameScene::* StateUpdateCaller)();

	template<cmGameState S>
	void StateClick(int x,int y,int E_BTN) {};
	void(cmGameScene::* StateClickCaller)(int x, int y, int E_BTN);
#pragma endregion

	// INIT (LOGIN)
	pgBtn* avatarBtn[4];
	pgBtn* confirmBtn;
	int selectedAvatar;
	const int idName = 100;
	HWND hName;
	void SendLoginSignal();
	// LOBBY
	HWND hChat;
	const int idChat = 101;
	std::vector<std::pair<int, bool>> lobbyInfo;
	std::list<std::wstring> chatBuf;
	pgBtn* refreshBtn;
	const int maxChatSize = 17;
	void SendChatMsg();
	void SendLobbyIn(int index);
	void SendLobbyRefMsg();
	TableArrange lobbyArrange;
	
	// Game
	std::wstring gameAnswer;
	std::vector<GameUserInfo> gameUserInfoVec;
	TableArrange gameArrange;
	pgBtn* readyBtn;
	pgBtn* exitBtn;
	pgBtn* sketchPlane;
	const int maxChatSizeInGame = 5;
	int TurnIdx;
	int MyIdx;
	bool isPlaying = false;
	bool reserveToRoomOut = false;
	bool isDrawingContinuous = false;
	void SendRdySignal();
	void SendGameExitSignal();
	void UpdateDroObj();
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

#pragma region Template Specialized Method
template<>
void cmGameScene::ProcessPacket<cmGameState::INIT>(int type, int size);

template<>
void cmGameScene::ProcessPacket<cmGameState::LOBBY>(int type, int size);

template<>
void cmGameScene::ProcessPacket<cmGameState::IN_GAME>(int type, int size);

template<>
void cmGameScene::StateDraw<cmGameState::INIT>(HDC hdc);

template<>
void cmGameScene::StateDraw<cmGameState::LOBBY>(HDC hdc);

template<>
void cmGameScene::StateDraw<cmGameState::IN_GAME>(HDC hdc);

template<>
void cmGameScene::StateInit<cmGameState::INIT>();

template<>
void cmGameScene::StateInit<cmGameState::LOBBY>();

template<>
void cmGameScene::StateInit<cmGameState::IN_GAME>();

template<>
void cmGameScene::StateRelease<cmGameState::INIT>();

template<>
void cmGameScene::StateRelease<cmGameState::LOBBY>();

template<>
void cmGameScene::StateRelease<cmGameState::IN_GAME>();

template<>
void cmGameScene::StateUpdate<cmGameState::INIT>();

template<>
void cmGameScene::StateUpdate<cmGameState::LOBBY>();

template<>
void cmGameScene::StateUpdate<cmGameState::IN_GAME>();

template<>
void cmGameScene::StateClick<cmGameState::INIT>(int x, int y, int E_BTN);

template<>
void cmGameScene::StateClick<cmGameState::LOBBY>(int x, int y, int E_BTN);

template<>
void cmGameScene::StateClick<cmGameState::IN_GAME>(int x, int y, int E_BTN);
#pragma endregion

