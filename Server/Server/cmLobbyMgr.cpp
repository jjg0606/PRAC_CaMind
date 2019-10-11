#include "cmLobbyMgr.h"

cmLobbyMgr cmLobbyMgr::instance;

cmLobbyMgr::cmLobbyMgr()
{

}

void cmLobbyMgr::getIn(cmGameUser* user)
{
	userMtx.lock();
	userSet.insert(user);
	userMtx.unlock();
}

void cmLobbyMgr::getOut(cmGameUser* user)
{
	userMtx.lock();
	userSet.erase(user);
	userMtx.unlock();
}

void cmLobbyMgr::BroadCast(void* source, int size)
{
	userMtx.lock_shared();
	for (auto iter = userSet.begin(); iter != userSet.end(); iter++)
	{
		(*iter)->SendPacket(source, size);
	}
	userMtx.unlock_shared();
}