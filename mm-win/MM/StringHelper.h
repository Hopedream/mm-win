
#pragma once

#include <windows.h>
#include <string>

using namespace std;
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
class CChineseCode
{
public:
	static void UTF_8ToUnicode(wchar_t* pOut, const char *pText);  // 把UTF-8转换成Unicode
	static void UnicodeToUTF_8(char* pOut,wchar_t* pText);  //Unicode 转换成UTF-8
	static void UnicodeToGB2312(char* pOut,wchar_t uData);  // 把Unicode 转换成 GB2312 
	static void Gb2312ToUnicode(wchar_t* pOut, const char *gbBuffer);// GB2312 转换成　Unicode
	static void GB2312ToUTF_8(string& pOut,const char *pText, int pLen);//GB2312 转为 UTF-8
	static void UTF_8ToGB2312(string &pOut,const char *pText, int pLen);//UTF-8 转为 GB2312
	static string EncodeUTF8(const string strInput);
	static string DecodeUTF8(const string strUtf8);
	static string str2HexStr(string str);
	static string hexStr2Str(string hexStr);
};