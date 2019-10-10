
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

	BroadCastRoomChange(&locPac,locPac.header.size);
	// TODO
	// send room info to sigle user 
	userSeat[seat] = user;
	std::pair<int, bool> userRoomState = std::make_pair(seat, false);
	userArr.insert(std::make_pair(user, userRoomState));
	
	return true;
}

void cmGameRoom::requestRoomOut(cmGameUser* user)
{
	std::unique_lock<std::shared_mutex> lckUserArr(mtxUserArr);
	auto iter = userArr.find(user);
	int seat = iter->second.first;
	userSeat[seat] = nullptr;
	userArr.erase(iter);

	PACKET_LOBBY_CHANGE locPac;
	locPac.header.type = PACKET_TYPE_LOBBY_CHANGE;
	locPac.header.size = sizeof(locPac);
	locPac.seatBefore = seat;
	locPac.seatAfter = -1;

	BroadCastRoomChange(&locPac,locPac.header.size);
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

void cmGameRoom::BroadCastRoomChange(void* source,int size)
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