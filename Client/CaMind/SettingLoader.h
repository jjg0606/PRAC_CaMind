#pragma once
#include <string>
#include <fstream>
#include <map>

class SettingLoader
{
private:
	static const std::string contextPath;

	bool isOK;
	int Port;
	int IPtype;
	int Protocol;
	std::ifstream ifs;
	std::map<std::string, std::string> settingVal;
	explicit SettingLoader();
	bool caseIgnoreMatch(const std::string& s1, const std::string& s2);
	void ToLower(std::string& str);
public:
	static SettingLoader instance;
	static int getPort();
	static int getIPType();
	static int getProtocol();
	static bool isLoaded();
	std::string& operator[](const std::string& key);
}; 
