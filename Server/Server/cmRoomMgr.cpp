#include "cmRoomMgr.h"

cmRoomMgr cmRoomMgr::instance;

cmRoomMgr::cmRoomMgr()
{

}

cmGameRoom* cmRoomMgr::operator[](int index)
{
	if (index >= 0 && index < MAX_LOBBY)
	{
		return roomArr + index;
	}
	return nullptr;
}

void cmRoomMgr::GetAllRoomInfo(cmGameUser* user)
{
	PACKET_LOBBY_INFO lobyinfo;
	lobyinfo.header.type = PACKET_TYPE_LOBBY_INFO;
	lobyinfo.header.size = sizeof(lobyinfo);
	lobyinfo.roomNum = MAX_LOBBY;

	for (int i = 0; i < MAX_LOBBY; i++)
	{
		roomArr[i].GetRoomInfo(lobyinfo.playerNum[i], lobyinfo.isPlaying[i]);
	}

	user->SendPacket(&lobyinfo, lobyinfo.header.size);
}