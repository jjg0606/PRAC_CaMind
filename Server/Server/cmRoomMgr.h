#pragma once
#include "cmGameRoom.h"
#include "cmGameUser.h"
#include "PACKET.h"

class cmGameRoom;
class cmGameUser;

class cmRoomMgr
{
	cmGameRoom roomArr[MAX_LOBBY];

	cmRoomMgr();
public:
	static cmRoomMgr instance;
	cmGameRoom* operator[](int index);
	void getAllRoomInfo(cmGameUser* user);
};