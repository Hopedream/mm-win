#ifndef _PUBLIC_FUNC_H_
#define _PUBLIC_FUNC_H_


#ifdef UNICODE
#define tstring  tstring
#else
#define tstring  string
#endif 


#pragma warning( disable:4786 ) //禁用这个警告
//#include <string>
#include <vector>
//#include <map>
//#include <set>
//#include <stack>
//#include <list>
//#include <deque>
#include <fstream>
#include <stdio.h>
#include <io.h>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

#define WM_MESSAGE_TIP WM_USER + 1000

class PublicFunc
{

public:


	// 获取程序路径
	static tstring GetProgramPath(void);

	// 取得系统我的文档文件路径，末尾加上了“\”
	static tstring GetMyDocunmentsPath(void);

public:
	// 文件夹是否存在
	static BOOL IsExistDir(const tstring &Dir);

	// 是否存在文件
	static BOOL IsExistFile(const tstring &FName);
	// 获取系统临时目录
	static tstring GetSystemTmpDir();

	// 创建文件夹
	static BOOL CreateDir(const tstring &FullDir);

	// 通过路径创建本地所需数据文件 有可能是加密和非加密两种
	//static BOOL CreateFileFromtstring(tstring wstrFilePath, tstring& wstrData);

	// 如果有运行，返回进程的PID，没运行返回0
	static DWORD PublicFunc::GetProcessIdFromName(LPCTSTR name);

public:
	// ansi - unicode
	static HRESULT AnsiToUnicode(LPCSTR pszA, LPWSTR* ppszW);

	// unicode - ansi
	static HRESULT UnicodeToAnsi(LPCWSTR pszW, LPSTR* ppszA);

public:
	//Convert full phone number to non-international district number
	static tstring FullNumberToShortNumber(tstring wstrFullNumber);

	//Convert non-international district number to full phone number
	static tstring ShortNumberToFullNumber(tstring wstrShortNumber);

	//Remove same number
	static vector<tstring> GetAllNumberNoRepeat(vector<tstring> vecAllNumber);

	//Convert Unicode16 LE to UTF8 
	static string _UnicodeToUTF8(wstring wstrUnicode);

	//Convert UTF8 LE to Unicode16 LE
	static wstring _UTF8ToUnicode(string strUtf8);

	//Convert Unicode16 LE to Ansi 
	static string _UnicodeToAnsi(wstring wstrUnicode);

	//Convert Ansi to Unicode16 LE
	static wstring _AnsiToUnicode(string strAnsi);

	static string UrlEncode(const string& str);	

	static string UrlDecode(const string& str);

	//Convert String to Uppercase
	static tstring ToUppercase(const tstring& wstrSource);
	
	static void Trim(tstring &strsrc);	

	static void TimeFormat(__int64 nMsgTime, tstring &strTime, bool bShowDetail = false);																				

	static DWORD GetFileSize(const tstring & strFilePath);

	// 通过资源ID,资源类型加载资源到内存，返回首地址指针
	static LPVOID GetResMemory(UINT IDResource, const tstring &strType, DWORD &dwSize);

	static void rc4_crypt(const tstring& wstrSrc, tstring& wstrDes);

public:

	// 获取号码在联系中名字,如果找不到则直接返回号码
	//static void GetContactName(tstring strPhoneNumber, tstring &strName);

	// 获取联系人ID
	//static void GetContactID(tstring strPhoneNumber, int& nID);
	//static void StringReplace(string &strBase, const string &strOld, const string &strNew);  	

	static void StringReplace(tstring &strBase, const tstring &strOld, const tstring &strNew); 

public:
	// 判断是否是中国的手机号码
	static BOOL IsChineseMobileNumber(const tstring strNumber);

public:
	// 合成免登录前缀URL
	//static tstring FormatAutoLoginPrefixURL(tstring wstrUserId, tstring wstrUserPwd);

	// 获得程序运行的系统及版本
	static bool GetOSTypeAndVersion(tstring& wstrOSType, tstring& wstrOSVersion);

	// 获得本机物理网卡地址
	static int GetMacAddress(vector<tstring> &LanMAC);

public:
	static tstring Uint64ToString(const unsigned __int64 nNumber);

	static unsigned __int64 StringToUint64(const tstring wstrNumber);

};


#endif //_PUBLIC_FUNC_H_
