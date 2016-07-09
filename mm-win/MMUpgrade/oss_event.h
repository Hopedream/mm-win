#pragma once
#include <string>
using namespace std;


class IOssEventBase
{
public:
	virtual void OnProcess(string& fileName, int iValue, string &strKey) = 0;
};