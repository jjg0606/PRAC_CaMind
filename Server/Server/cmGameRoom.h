#pragma once
#include <shared_mutex>
#include <list>
#include <map>
#include "cmGameUser.h"
#include "PACKET.h"
class cmGameUser;

typedef struct
{
	int avatar;
	bool isReady;
	int seat;
}
UserRoomInfo;

class cmGameRoom
{
	bool isPlaying;
	std::wstring answer;
	std::shared_mutex mtxUserArr;
	cmGameUser* userSeat[MAX_PLAYERS_IN_ROOM];
	// avatar, isReady
	std::map<cmGameUser*, UserRoomInfo> userArr;

	int findEmptySeat();
	void SendRoomInfoTo(cmGameUser* user);
public:
	cmGameRoom();
	bool requestRoomIn(cmGameUser* user);
	void requestRoomOut(cmGameUser* user);
	void getRoomInfo(int& userNum, bool& isPlaying);
	void ReadyStateChange(cmGameUser* user, bool isReady);
	void BroadCastToRoomUser(void* source,int size);
};