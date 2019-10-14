
#include "cmGameRoom.h"
#include "cmAnswerReader.h"

const int cmGameRoom::DefaultTurnIdx = -1;

cmGameRoom::cmGameRoom()
{
	for (int i = 0; i < MAX_PLAYERS_IN_ROOM; i++)
	{
		userSeat[i] = nullptr;
	}
}

#pragma region Util function
int cmGameRoom::FindEmptySeat()
{
	for (int i = 0; i < MAX_PLAYERS_IN_ROOM; i++)
	{
		if (userSeat[i] == nullptr)
		{
			return i;
		}
	}
}

bool cmGameRoom::isStartCondition()
{
	if (userArr.size() < 2)
	{
		return false;
	}

	mtxUserArr.lock_shared();
	bool retVal = true;
	for (auto iter = userArr.begin(); iter != userArr.end(); iter++)
	{
		if (iter->second.isReady == false)
		{
			retVal = false;
			break;
		}
	}
	mtxUserArr.unlock_shared();

	return retVal;
}

void cmGameRoom::SetUnreadyAll()
{
	mtxUserArr.lock();
	for (auto iter = userArr.begin(); iter != userArr.end(); iter++)
	{
		iter->second.isReady = false;
	}
	mtxUserArr.unlock();
}

int cmGameRoom::FindNextTurnIdx(int curTurn)
{
	int start = curTurn >= 0 && curTurn < MAX_PLAYERS_IN_ROOM ? curTurn+1 : 0;
	for (int i = start; i < MAX_PLAYERS_IN_ROOM; i++)
	{
		if (userSeat[i] != nullptr)
		{
			return i;
		}
	}

	for (int i = 0; i < start; i++)
	{
		if (userSeat[i] != nullptr)
		{
			return i;
		}
	}

	// error case
	return -1;
}
#pragma endregion

#pragma region IO Function
void cmGameRoom::SendRoomChangeMsg(cmGameUser* user, int seatBefore, int seatAfter,bool isReady)
{
	PACKET_LOBBY_CHANGE locPac;
	memset(&locPac, 0, sizeof(locPac));
	locPac.header.type = PACKET_TYPE_LOBBY_CHANGE;
	locPac.header.size = sizeof(locPac);
	locPac.seatBefore = seatBefore;
	locPac.seatAfter = seatAfter;
	locPac.avatar = user->GetAvatar();
	locPac.rdyState = isReady;
	user->GetNameCopy(locPac.name);

	BroadCastToRoomUser(&locPac, locPac.header.size);
}

void cmGameRoom::SendRoomInfoTo(cmGameUser* user)
{
	// already have lock
	PACKET_LOBBY_PLAYERS lobppac;
	memset(&lobppac, 0, sizeof(lobppac));
	lobppac.header.type = PACKET_TYPE_LOBBY_PLAYERS;
	lobppac.header.size = sizeof(lobppac);
	lobppac.seatIdx = userArr[user].seat;
	for (int i = 0; i < MAX_PLAYERS_IN_ROOM; i++)
	{
		if (userSeat[i] == nullptr)
		{
			lobppac.avatar[i] = cmGameUser::DefaultAvatarNum;
			continue;
		}
		cmGameUser* cur = userSeat[i];
		cur->GetNameCopy(lobppac.nameArr[i]);
		lobppac.avatar[i] = userArr[cur].avatar;
		lobppac.isReady[i] = userArr[cur].isReady;
	}
	user->SendPacket(&lobppac, lobppac.header.size);
}

void cmGameRoom::BroadCastTurnMsg()
{
	PACKET_GAME_TURN_MSG turnPac;
	turnPac.header.type = PACKET_TYPE_GAME_TURN_MSG;
	turnPac.header.size = sizeof(turnPac);
	turnPac.turnp = turnIdx;
	BroadCastToRoomUser(&turnPac, turnPac.header.size);
}

void cmGameRoom::BroadCastGameEnd()
{
	PACKET_SYSTEM syspac;
	syspac.header.type = PACKET_TYPE_SYSTEM;
	syspac.header.size = sizeof(syspac);
	syspac.system_msg = SYSTEM_MSG_GAME_END;
	BroadCastToRoomUser(&syspac, syspac.header.size);	
}

void cmGameRoom::SendAnswerTo(cmGameUser* user)
{
	PACKET_GAME_ANSWER ansPac;
	memset(&ansPac, 0, sizeof(ansPac));
	ansPac.header.type = PAKCET_TYPE_GAME_ANSWER;
	ansPac.header.size = sizeof(ansPac);
	for (int i = 0; i < answer.length(); i++)
	{
		ansPac.answer[i] = answer[i];
	}

	user->SendPacket(&ansPac, ansPac.header.size);
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

void cmGameRoom::BroadCastToRoomUserExcept(cmGameUser* user, void* source, int size)
{
	for (int i = 0; i < MAX_PLAYERS_IN_ROOM; i++)
	{
		if (userSeat[i] == nullptr || userSeat[i] == user)
		{
			continue;
		}

		userSeat[i]->SendPacket(source, size);
	}
}
#pragma endregion

#pragma region Game Process Function
void cmGameRoom::StartGame()
{
	if (isPlaying)
	{
		return;
	}

	isPlaying = true;
	answer = cmAnswerReader::instance.GetAnswer();
	turnIdx = FindNextTurnIdx(DefaultTurnIdx);
	BroadCastTurnMsg();
	SendAnswerTo(userSeat[turnIdx]);
}

void cmGameRoom::EndGame()
{
	if (!isPlaying)
	{
		return;
	}

	isPlaying = false;
	BroadCastGameEnd();
	SetUnreadyAll();
}

void cmGameRoom::ProcessAnswer()
{
	turnIdx = FindNextTurnIdx(turnIdx);
	answer = cmAnswerReader::instance.GetAnswer();
	BroadCastTurnMsg();
	SendAnswerTo(userSeat[turnIdx]);
	
}
#pragma endregion
/////// public /////////
#pragma region cmUser Interface
bool cmGameRoom::RequestRoomIn(cmGameUser* user)
{
	std::unique_lock<std::shared_mutex> lckUserArr(mtxUserArr);
	if (userArr.size() >= MAX_PLAYERS_IN_ROOM || isPlaying)
	{
		return false;
	}
	int seat = FindEmptySeat();

	SendRoomChangeMsg(user, -1, seat,false);
	 
	userSeat[seat] = user;
	UserRoomInfo roominfo;
	roominfo.avatar = user->GetAvatar();
	roominfo.isReady = false;
	roominfo.seat = seat;
	userArr.insert(std::make_pair(user,roominfo));
	// send room info to sigle user
	SendRoomInfoTo(user);
	
	return true;
}

void cmGameRoom::RequestRoomOut(cmGameUser* user)
{
	std::unique_lock<std::shared_mutex> lckUserArr(mtxUserArr);
	if (userArr.find(user) == userArr.end())
	{
		return;
	}

	int seat = userArr[user].seat;
	userSeat[seat] = nullptr;
	userArr.erase(user);

	SendRoomChangeMsg(user, seat, -1, false);
	lckUserArr.unlock();
	EndGame();
}

void cmGameRoom::ToggleReady(cmGameUser* user)
{
	if (isPlaying)
	{
		return;
	}

	std::unique_lock<std::shared_mutex> lckUserArr(mtxUserArr);
	auto iter = userArr.find(user);
	if (iter == userArr.end())
	{
		return;
	}

	UserRoomInfo& roomInfo = iter->second;
	roomInfo.isReady = !roomInfo.isReady;

	lckUserArr.unlock();

	SendRoomChangeMsg(user, roomInfo.seat, roomInfo.seat, roomInfo.isReady);

	if (isStartCondition())
	{
		StartGame();
	}
}


void cmGameRoom::BroadCastChatMsg(cmGameUser* user, void* source, int size)
{
	std::shared_lock<std::shared_mutex> slUserArr(mtxUserArr);
	auto iter = userArr.find(user);
	if (iter == userArr.end())
	{
		return;
	}

	slUserArr.unlock();
	BroadCastToRoomUser(source, size);
}

void cmGameRoom::PointsPacketSync(cmGameUser* user, void* source, int size)
{
	std::shared_lock<std::shared_mutex> slUserArr(mtxUserArr);
	auto iter = userArr.find(user);
	if (iter == userArr.end())
	{
		return;
	}

	if (iter->second.seat != turnIdx)
	{
		return;
	}

	BroadCastToRoomUserExcept(user, source, size);
}

void cmGameRoom::PointClearOrder(cmGameUser* user, void* source, int size)
{
	std::shared_lock<std::shared_mutex> slUserArr(mtxUserArr);
	auto iter = userArr.find(user);
	if (iter == userArr.end())
	{
		return;
	}

	if (iter->second.seat != turnIdx)
	{
		return;
	}

	BroadCastToRoomUserExcept(user, source, size);
}

void cmGameRoom::ChkAnswer(cmGameUser* user, std::wstring& ans)
{
	if (!isPlaying)
	{
		return;
	}

	std::shared_lock<std::shared_mutex> slUserArr(mtxUserArr);
	auto iter = userArr.find(user);
	if (iter == userArr.end())
	{
		return;
	}

	if (iter->second.seat == turnIdx)
	{
		return;
	}
	slUserArr.unlock();

	if (ans == answer)
	{
		ProcessAnswer();
	}	
}
#pragma endregion

void cmGameRoom::GetRoomInfo(int& userNum, bool& isPlaying)
{
	std::shared_lock<std::shared_mutex> lckUserArr(mtxUserArr);
	userNum = userArr.size();
	isPlaying = this->isPlaying;
}
