#ifndef _PUBLIC_FUNC_H_
#define _PUBLIC_FUNC_H_


#ifdef UNICODE
#define tstring  tstring
#else
#define tstring  string
#endif 


#pragma warning( disable:4786 ) //�����������
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


	// ��ȡ����·��
	static tstring GetProgramPath(void);

	// ȡ��ϵͳ�ҵ��ĵ��ļ�·����ĩβ�����ˡ�\��
	static tstring GetMyDocunmentsPath(void);

public:
	// �ļ����Ƿ����
	static BOOL IsExistDir(const tstring &Dir);

	// �Ƿ�����ļ�
	static BOOL IsExistFile(const tstring &FName);
	// ��ȡϵͳ��ʱĿ¼
	static tstring GetSystemTmpDir();

	// �����ļ���
	static BOOL CreateDir(const tstring &FullDir);

	// ͨ��·�������������������ļ� �п����Ǽ��ܺͷǼ�������
	//static BOOL CreateFileFromtstring(tstring wstrFilePath, tstring& wstrData);

	// ��������У����ؽ��̵�PID��û���з���0
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

	// ͨ����ԴID,��Դ���ͼ�����Դ���ڴ棬�����׵�ַָ��
	static LPVOID GetResMemory(UINT IDResource, const tstring &strType, DWORD &dwSize);

	static void rc4_crypt(const tstring& wstrSrc, tstring& wstrDes);

public:

	// ��ȡ��������ϵ������,����Ҳ�����ֱ�ӷ��غ���
	//static void GetContactName(tstring strPhoneNumber, tstring &strName);

	// ��ȡ��ϵ��ID
	//static void GetContactID(tstring strPhoneNumber, int& nID);
	//static void StringReplace(string &strBase, const string &strOld, const string &strNew);  	

	static void StringReplace(tstring &strBase, const tstring &strOld, const tstring &strNew); 

public:
	// �ж��Ƿ����й����ֻ�����
	static BOOL IsChineseMobileNumber(const tstring strNumber);

public:
	// �ϳ����¼ǰ׺URL
	//static tstring FormatAutoLoginPrefixURL(tstring wstrUserId, tstring wstrUserPwd);

	// ��ó������е�ϵͳ���汾
	static bool GetOSTypeAndVersion(tstring& wstrOSType, tstring& wstrOSVersion);

	// ��ñ�������������ַ
	static int GetMacAddress(vector<tstring> &LanMAC);

public:
	static tstring Uint64ToString(const unsigned __int64 nNumber);

	static unsigned __int64 StringToUint64(const tstring wstrNumber);

};


#endif //_PUBLIC_FUNC_H_
