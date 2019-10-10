#pragma once
#include <shared_mutex>
#include <list>
#include <map>
#include "cmGameUser.h"
#include "PACKET.h"
class cmGameUser;



class cmGameRoom
{
	bool isPlaying;
	std::shared_mutex mtxUserArr;
	cmGameUser* userSeat[MAX_PLAYERS_IN_ROOM];
	std::map<cmGameUser*, std::pair<int,bool>> userArr;

	int findEmptySeat();
	void BroadCastRoomChange(void* source,int size);
public:
	cmGameRoom();
	bool requestRoomIn(cmGameUser* user);
	void requestRoomOut(cmGameUser* user);
	void getRoomInfo(int& userNum, bool& isPlaying);
	void ReadyStateChange(cmGameUser* user, bool isReady);
};