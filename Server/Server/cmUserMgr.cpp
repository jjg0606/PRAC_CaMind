#include "cmUserMgr.h"

cmUserMgr cmUserMgr::instance;

void cmUserMgr::Insert(SOCKET sock, std::string client_addr, int client_port)
{
	userMap.insert(std::make_pair(sock,new cmGameUser(sock,client_addr,client_port)));
}

void cmUserMgr::Delete(SOCKET soc)
{
	auto iter = userMap.find(soc);
	if (iter == userMap.end())
	{
		return;
	}

	delete iter->second;
	userMap.erase(iter);
}

cmGameUser* cmUserMgr::operator[](SOCKET soc)
{
	auto iter = userMap.find(soc);
	if (iter == userMap.end())
	{
		return nullptr;
	}
	
	return iter->second;
}

cmUserMgr::~cmUserMgr()
{
	for (auto iter = userMap.begin(); iter != userMap.end(); iter++)
	{
		delete iter->second;
	}

	userMap.clear();
}