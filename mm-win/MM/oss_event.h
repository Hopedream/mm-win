#pragma once
using namespace std;
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}

class IOssEventBase
{
public:
	virtual void OnProcess(tstring& fileName, int iValue, tstring &strKey) = 0;
};