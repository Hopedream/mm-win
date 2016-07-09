#pragma once
#include <string>
#include <regex>
using namespace std;
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
class CRegexHelper{
public:
	static bool IsValidEmailAddr(const char* strEmailAddr);
	static bool IsValidMobileNo(const char* strPhoneNo);
	static bool IsFirstPYMatch(const char* strText, const char* pattern);
	static void GenFirstPYLetter(std::tstring strName, std::tstring&strResult );
	static bool IsContainChineseCharacter(tstring strTxt);
	static bool IsEmptyString(tstring strTxt);
	static tstring ConvertChineseUnicodeToPyt(tstring strText);
	static bool IsValidOrgName(tstring strText);
private:	
	static void FirstLetter(int nCode, std::tstring& strLetter);
};
