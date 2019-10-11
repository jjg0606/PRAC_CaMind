#pragma once
#include <string>
#include <vector>

class cmAnswerReader
{
	const std::string context = "./answer.txt";
	cmAnswerReader();
	std::vector<std::wstring> answerVec;
public:
	std::wstring& getAnswer();
	static cmAnswerReader instance;
};