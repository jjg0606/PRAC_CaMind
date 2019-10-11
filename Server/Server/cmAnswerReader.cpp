#include "cmAnswerReader.h"
#include <fstream>
#include <random>
#include <time.h>
#include <locale.h>

using namespace std;

cmAnswerReader cmAnswerReader::instance;

cmAnswerReader::cmAnswerReader()
{
	srand(time(NULL));

	ifstream ifs;
	ifs.open(context, ios::in);

	if (!ifs.good())
	{
		exit(-1);
	}
	setlocale(LC_ALL, "korean");
	char buf[50];
	wchar_t wbuf[50];
	while (!ifs.eof())
	{
		ifs.getline(buf, 50);

		mbstowcs(wbuf, buf, 50);
		wstring cur(wbuf);
		if (cur.length() >= 1)
		{
			answerVec.push_back(cur);
		}		
	}

	ifs.close();
}

wstring& cmAnswerReader::getAnswer()
{
	int index = rand() % answerVec.size();
	return answerVec[index];
}

