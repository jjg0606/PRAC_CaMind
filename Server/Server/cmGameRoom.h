#pragma once
#include <shared_mutex>
#include <list>
#include <map>
#include "cmGameUser.h"
#include "PACKET.h"
class cmGameUser;



class cmGameRoom
{
	typedef struct
	{
		int avatar;
		bool isReady;
		int seat;
	}
	UserRoomInfo;

	bool isPlaying = false;
	int turnIdx;
	static const int DefaultTurnIdx;
	std::wstring answer;
	std::shared_mutex mtxUserArr;
	cmGameUser* userSeat[MAX_PLAYERS_IN_ROOM];
	std::map<cmGameUser*, UserRoomInfo> userArr;

	int FindEmptySeat();
	void SendRoomChangeMsg(cmGameUser* user,int seatBefore,int seatAfter,bool isReady);
	void SendRoomInfoTo(cmGameUser* user);

	bool isStartCondition();
	void StartGame();
	void EndGame();
	void SetUnreadyAll();
	int FindNextTurnIdx(int curTurn);
	void BroadCastTurnMsg();
	void BroadCastGameEnd();
	void SendAnswerTo(cmGameUser* user);
	void BroadCastToRoomUserExcept(cmGameUser* user, void* source, int size);
	void ProcessAnswer();
public:
	cmGameRoom();
	bool RequestRoomIn(cmGameUser* user);
	void RequestRoomOut(cmGameUser* user);
	void ToggleReady(cmGameUser* user);
	void GetRoomInfo(int& userNum, bool& isPlaying);
	void BroadCastToRoomUser(void* source,int size);
	void PointsPacketSync(cmGameUser* user, void* source, int size);
	void ChkAnswer(cmGameUser* user,std::wstring& ans);
	
};