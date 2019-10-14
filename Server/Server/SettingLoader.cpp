#include "SettingLoader.h"
#include <regex>
#include <memory>
#include <WinSock2.h>
using namespace std;

const string SettingLoader::contextPath = "./setting.txt";

SettingLoader SettingLoader::instance;

SettingLoader::SettingLoader()
{
	this->Port = -1;
	this->IPtype = -1;
	this->Protocol = -1;

	ifs.open(contextPath, ios::in);
	if (!ifs.good())
	{
		ifs.close();
		return;
	}

	regex rCapture(R"((\w+)\s*=\s*([\w.]+))");

	int length = 0;
	ifs.seekg(0, ios::end);
	length = ifs.tellg();
	ifs.seekg(0, ios::beg);

	char* buf = new char[length];
	ifs.read(buf, length);

	cregex_iterator end;

	for (cregex_iterator iter(buf, buf + length - 1, rCapture); iter != end; iter++)
	{
		string key = (*iter)[1];
		string value = (*iter)[2];
		ToLower(key);
		ToLower(value);
		settingVal[key] = value;
	}

	// port
	this->Port = stoi(settingVal["port"]);
	// iptype
	if (settingVal["iptype"] == "ipv4")
	{
		this->IPtype = AF_INET;
	}
	else if (settingVal["iptype"] == "ipv6")
	{
		this->IPtype = AF_INET6;
	}
	// protocol
	if (settingVal["protocol"] == "tcp")
	{
		this->Protocol = SOCK_STREAM;
	}
	else if (settingVal["protocol"] == "udp")
	{
		this->Protocol = SOCK_DGRAM;
	}
	//max thread

	ifs.close();

	isOK = Port != -1 && IPtype != -1 && Protocol != -1;
}

bool SettingLoader::isLoaded()
{
	return instance.isOK;
}

int SettingLoader::getPort()
{
	return instance.Port;
}

int SettingLoader::getIPType()
{
	return instance.IPtype;
}

int SettingLoader::getProtocol()
{
	return instance.isOK;
}

bool SettingLoader::caseIgnoreMatch(const string& s1, const string& s2)
{
	if (s1.length() != s2.length())
	{
		return false;
	}
	int dist = 'a' - 'A';
	for (int i = 0; i < s1.length(); i++)
	{
		char c1 = s1[i];
		char c2 = s2[i];
		c1 = c1 > 'Z' ? c1 - dist : c1;
		c2 = c2 > 'Z' ? c2 - dist : c2;
		if (c1 != c2)
		{
			return false;
		}
	}
	return true;
}

void SettingLoader::ToLower(string& str)
{
	for (int i = 0; i < str.length(); i++)
	{
		str[i] = tolower(str[i]);
	}
}

string& SettingLoader::operator[](const std::string& key)
{
	return settingVal[key];
}