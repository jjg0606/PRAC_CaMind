
#include "cmGameRoom.h"

cmGameRoom::cmGameRoom()
{
	isPlaying = false;
	for (int i = 0; i < MAX_PLAYERS_IN_ROOM; i++)
	{
		userSeat[i] = nullptr;
	}
}

bool cmGameRoom::requestRoomIn(cmGameUser* user)
{
	std::unique_lock<std::shared_mutex> lckUserArr(mtxUserArr);
	if (userArr.size() >= MAX_PLAYERS_IN_ROOM)
	{
		return false;
	}
	int seat = findEmptySeat();

	PACKET_LOBBY_CHANGE locPac;
	locPac.header.type = PACKET_TYPE_LOBBY_CHANGE;
	locPac.header.size = sizeof(locPac);
	locPac.seatBefore = -1;
	locPac.seatAfter = seat;
	locPac.avatar = user->getAvatar();
	locPac.rdyState = false;
	user->getNameCopy(locPac.name);

	BroadCastToRoomUser(&locPac,locPac.header.size);
	// TODO
	// send room info to sigle user 
	userSeat[seat] = user;
	UserRoomInfo roominfo;
	roominfo.avatar = user->getAvatar();
	roominfo.isReady = false;
	roominfo.seat = seat;
	userArr.insert(std::make_pair(user,roominfo));
	
	SendRoomInfoTo(user);

	return true;
}

void cmGameRoom::requestRoomOut(cmGameUser* user)
{
	std::unique_lock<std::shared_mutex> lckUserArr(mtxUserArr);
	if (userArr.find(user) == userArr.end())
	{
		return;
	}	

	int seat = userArr[user].seat;
	userSeat[seat] = nullptr;	
	userArr.erase(user);

	PACKET_LOBBY_CHANGE locPac;
	locPac.header.type = PACKET_TYPE_LOBBY_CHANGE;
	locPac.header.size = sizeof(locPac);
	locPac.seatBefore = seat;
	locPac.seatAfter = -1;

	BroadCastToRoomUser(&locPac,locPac.header.size);
}

int cmGameRoom::findEmptySeat()
{
	for (int i = 0; i < MAX_PLAYERS_IN_ROOM; i++)
	{
		if (userSeat[i] == nullptr)
		{
			return i;
		}
	}
}

void cmGameRoom::BroadCastToRoomUser(void* source,int size)
{
	for (int i = 0; i < MAX_PLAYERS_IN_ROOM; i++)
	{
		if (userSeat[i] == nullptr)
		{
			continue;
		}

		userSeat[i]->SendPacket(source, size);
	}
}

void cmGameRoom::getRoomInfo(int& userNum, bool& isPlaying)
{
	std::shared_lock<std::shared_mutex> lckUserArr(mtxUserArr);
	userNum = userArr.size();
	isPlaying = this->isPlaying;
}

void cmGameRoom::ReadyStateChange(cmGameUser* user, bool isReady)
{
	std::unique_lock<std::shared_mutex> lckUserArr(mtxUserArr);
	
}

void cmGameRoom::SendRoomInfoTo(cmGameUser* user)
{
	// already have lock
	PACKET_LOBBY_PLAYERS lobppac;
	lobppac.header.type = PACKET_TYPE_LOBBY_PLAYERS;
	lobppac.header.size = sizeof(lobppac);
	lobppac.cntPlayer = userArr.size();
	for (int i = 0; i < MAX_PLAYERS_IN_ROOM; i++)
	{
		if (userSeat[i] == nullptr)
		{
			lobppac.avatar[i] = -1;
			continue;
		}
		cmGameUser* cur = userSeat[i];
		cur->getNameCopy(lobppac.nameArr[i]);
		lobppac.avatar[i] = userArr[cur].avatar;
		lobppac.isReady[i] = userArr[cur].isReady;
	}
	user->SendPacket(&lobppac, lobppac.header.size);
}