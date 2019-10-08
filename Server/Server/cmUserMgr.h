#pragma once
#include "cmGameUser.h"
#include <map>

class cmUserMgr
{
private:
	std::map<SOCKET, cmGameUser*> userMap;
	cmUserMgr() {};
public:
	static cmUserMgr instance;

	void Insert(SOCKET sock, std::string client_addr, int client_port);
	void Delete(SOCKET soc);
	cmGameUser* operator[](SOCKET soc);
	~cmUserMgr();
};