#pragma once
#include <set>
#include <shared_mutex>
#include "cmGameUser.h"

class cmGameUser;

class cmLobbyMgr
{
	std::shared_mutex userMtx;
	std::set<cmGameUser*> userSet;
	cmLobbyMgr();
public:
	static cmLobbyMgr instance;
	void GetIn(cmGameUser* user);
	void GetOut(cmGameUser* user);
	void BroadCast(void* source, int size);
};