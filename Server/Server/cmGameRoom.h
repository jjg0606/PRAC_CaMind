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

#pragma region Util function
	int FindEmptySeat();
	bool isStartCondition();
	void SetUnreadyAll();
	int FindNextTurnIdx(int curTurn);
#pragma endregion

#pragma region IO Functoin
	void SendRoomChangeMsg(cmGameUser* user,int seatBefore,int seatAfter,bool isReady);
	void SendRoomInfoTo(cmGameUser* user);
	void BroadCastTurnMsg();
	void BroadCastGameEnd();
	void SendAnswerTo(cmGameUser* user);
	void BroadCastToRoomUser(void* source,int size);
	void BroadCastToRoomUserExcept(cmGameUser* user, void* source, int size);
#pragma endregion

#pragma region Game Process Function
	void StartGame();
	void EndGame();
	void ProcessAnswer();
#pragma endregion	
public:
	cmGameRoom();
#pragma region cmUser Interface
	// called by cm User
	bool RequestRoomIn(cmGameUser* user);
	void RequestRoomOut(cmGameUser* user);
	void ToggleReady(cmGameUser* user);
	void BroadCastChatMsg(cmGameUser* user, void* source, int size);
	void PointsPacketSync(cmGameUser* user, void* source, int size);
	void PointClearOrder(cmGameUser* user,void* source,int size);
	void ChkAnswer(cmGameUser* user,std::wstring& ans);
#pragma endregion


	// called by roomMgr
	void GetRoomInfo(int& userNum, bool& isPlaying);
	
};