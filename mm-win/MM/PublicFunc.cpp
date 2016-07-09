#include "stdafx.h"
#include "PublicFunc.h"
#include <Tlhelp32.h>
#include <Shlobj.h>
//#include "PublicDefine.h"
#include <assert.h>
#include <string>
#include <time.h>
using namespace std;


// 获取程序路径
tstring PublicFunc::GetProgramPath(void)
{
	tstring strPath = _T("");

	TCHAR fileName[MAX_PATH];
	memset(fileName, 0, sizeof(fileName));

	if (!GetModuleFileName(NULL, fileName, sizeof(fileName)))
		return FALSE;

	strPath = fileName;
	int nPos = strPath.rfind('\\');
	strPath = strPath.substr(0, nPos);

	return strPath;
}

// 取得系统我的文档文件路径，末尾加上了“\”
tstring PublicFunc::GetMyDocunmentsPath(void)
{
	tstring strPath = "";

	// 取得系统我的文档目录
	TCHAR   szDocument[MAX_PATH]={0};   
	LPITEMIDLIST pidl = NULL;   
	SHGetSpecialFolderLocation(NULL, CSIDL_PERSONAL, &pidl);

	if (NULL != pidl)   
	{   
		SHGetPathFromIDList(pidl, szDocument);
	}

	strPath = szDocument;
	strPath += _T("\\");;

	return strPath;
}

// 创建文件夹,路径下，不存在则都创建
static BOOL _CREATE_DIR(const tstring &StdDir)
{
	BOOL bR = TRUE;

	tstring StrTmp;
	tstring DirUse;
	DirUse = StdDir;

	int iFind,offset;
	tstring Buf_;
	for (iFind = 0,offset = 0,Buf_ = DirUse; ; )
	{
		iFind = Buf_.find('\\');
		if (iFind == tstring::npos)
			break;
		StrTmp = DirUse.substr(0, offset + iFind);	
		offset += iFind + 1;
		if (!StrTmp.empty() && !PublicFunc::IsExistDir(StrTmp)) 
		{
			::CreateDirectory(StrTmp.c_str(),NULL);
		}
		Buf_ = Buf_.substr(iFind + 1, Buf_.length());
	}

	::CreateDirectory(DirUse.c_str(),NULL);

	return bR;
}

// 文件夹是否存在
BOOL PublicFunc::IsExistDir(const tstring &Dir)
{
	BOOL bR = TRUE;
	DWORD dwAttribute = ::GetFileAttributes(Dir.c_str());
	if (dwAttribute != -1)
	{
		bR = (dwAttribute & FILE_ATTRIBUTE_DIRECTORY);
	}
	else
	{
		bR = FALSE;
	}
	return bR;
}

// 文件是否存在
BOOL PublicFunc::IsExistFile(const tstring &FName)
{
	BOOL bR = TRUE;
	DWORD dwAttribute = ::GetFileAttributes(FName.c_str());
	if (dwAttribute != -1)
	{
		bR = !(dwAttribute & FILE_ATTRIBUTE_DIRECTORY);
	}
	else
	{
		bR = FALSE;
	}
	return bR; 
}

tstring PublicFunc::GetSystemTmpDir()
{
	tstring strTmpDir;

	TCHAR sztempdirectory[MAX_PATH] = {0};
	int i = GetTempPath(MAX_PATH - 1, sztempdirectory);
	//if ( i > 0 && sztempdirectory[i - 1] != "\\")
	//{
	//	wcscat_s(sztempdirectory, MAX_PATH, _"\\"));
	//}

	strTmpDir = sztempdirectory;
	return strTmpDir;
}
// 创建文件夹，不能以'\'结尾
BOOL PublicFunc::CreateDir(const tstring &FullDir)
{
	//if (FullDir.empty()) //禁用
	//{
	//	return FALSE;
	//}

	////tstring DirUse = FullDir;
	////if (((DirUse.length() > 0) && DirUse[DirUse.length() - 1] ==_T("\\")))
	////{//规范FullDir不能以'\'结尾
	////	DirUse = DirUse.substr(0, DirUse.length() - 1);
	////}
	//return _CREATE_DIR(DirUse);
	return FALSE;
}

// 查找进程是否运行
// 如果有运行，返回进程的PID，没运行返回0
DWORD PublicFunc::GetProcessIdFromName(LPCTSTR name) 
{ 
	/*PROCESSENTRY32 pe; 
	DWORD id = 0; 
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0); 
	pe.dwSize = sizeof(PROCESSENTRY32); 
	if( !Process32First(hSnapshot,&pe) ) 
		return 0; 
	while(1) 
	{ 
		pe.dwSize = sizeof(PROCESSENTRY32); 
		if( Process32Next(hSnapshot,&pe)==FALSE ) 
			break; 
		if(wcscmp(pe.szExeFile,name) == 0) 
		{ 
			id = pe.th32ProcessID; 
			break; 
		} 

	}
	CloseHandle(hSnapshot); 
	return id; */
	return -1;
}

/*
* AnsiToUnicode converts the ANSI string pszA to a Unicode string
* and returns the Unicode string through ppszW. Space for the
* the converted string is allocated by AnsiToUnicode.
*/ 
HRESULT PublicFunc::AnsiToUnicode(LPCSTR pszA, LPWSTR* ppszW)
{
	ULONG cCharacters;
	DWORD dwError;

	// If input is null then just return the same.
	if (NULL == pszA)
	{
		*ppszW = NULL;
		return NOERROR;
	}

	// Determine number of wide characters to be allocated for the
	// Unicode string.
	cCharacters =  strlen(pszA)+1;

	// Use of the OLE allocator is required if the resultant Unicode
	// string will be passed to another COM component and if that
	// component will free it. Otherwise you can use your own allocator.
	*ppszW = (LPOLESTR) CoTaskMemAlloc(cCharacters*2);
	if (NULL == *ppszW)
		return E_OUTOFMEMORY;

	// Covert to Unicode.
	if (0 == MultiByteToWideChar(CP_ACP, 0, pszA, cCharacters,
		*ppszW, cCharacters))
	{
		dwError = GetLastError();
		CoTaskMemFree(*ppszW);
		*ppszW = NULL;
		return HRESULT_FROM_WIN32(dwError);
	}

	return NOERROR;
}

/*
* UnicodeToAnsi converts the Unicode string pszW to an ANSI string
* and returns the ANSI string through ppszA. Space for the
* the converted string is allocated by UnicodeToAnsi.
*/ 
HRESULT PublicFunc::UnicodeToAnsi(LPCWSTR pszW, LPSTR* ppszA)
{
	ULONG cbAnsi, cCharacters;
	DWORD dwError;

	// If input is null then just return the same.
	if (pszW == NULL)
	{
		*ppszA = NULL;
		return NOERROR;
	}

	cCharacters = wcslen(pszW)+1;
	// Determine number of bytes to be allocated for ANSI string. An
	// ANSI string can have at most 2 bytes per character (for Double
	// Byte Character Strings.)
	cbAnsi = cCharacters*2;

	// Use of the OLE allocator is not required because the resultant
	// ANSI  string will never be passed to another COM component. You
	// can use your own allocator.
	*ppszA = (LPSTR) CoTaskMemAlloc(cbAnsi);
	if (NULL == *ppszA)
		return E_OUTOFMEMORY;

	// Convert to ANSI.
	if (0 == WideCharToMultiByte(CP_ACP, 0, pszW, cCharacters, *ppszA,
		cbAnsi, NULL, NULL))
	{
		dwError = GetLastError();
		CoTaskMemFree(*ppszA);
		*ppszA = NULL;
		return HRESULT_FROM_WIN32(dwError);
	}

	return NOERROR;
}
//************************************
// Method:    Convert Unicode string to UTF8
// FullName:  _UnicodeToUTF8
// Returns:   string
// Qualifier: protected
// Parameter: tstring
//				tstring Unicode
//************************************
string PublicFunc::_UnicodeToUTF8(wstring wstrUnicode)
{
	// unicode to UTF8
	//Check the len
	if (wstrUnicode.empty())
	{
		return string();
	}

	//Get len
	int nUff8Len = ::WideCharToMultiByte(CP_UTF8, NULL, wstrUnicode.c_str(), wstrUnicode.length(), NULL, 0, NULL, NULL);
	char* pcUtf8 = new char[nUff8Len + 1];

	//Convert
	::WideCharToMultiByte(CP_UTF8, NULL, wstrUnicode.c_str(), wstrUnicode.length(), pcUtf8, nUff8Len, NULL, NULL);
	pcUtf8[nUff8Len] = '\0';

	string strUtf8 = pcUtf8;

	return strUtf8;
}

//************************************
// Method:    Convert UTF8 string to Unicode
// FullName:  _UTF8ToUnicode
// Returns:   tstring
// Qualifier: protected
// Parameter: string
//				string UTF8
//************************************
wstring PublicFunc::_UTF8ToUnicode(string strUtf8)
{
	//UTF8 to Unicode
	//Check the len
	if (strUtf8.empty())
	{
		return wstring();
	}

	//Get len
	int nUniLen = ::MultiByteToWideChar(CP_UTF8, NULL, strUtf8.c_str(), strUtf8.length(), NULL, 0);
	WCHAR* pwcsUniString = new WCHAR[nUniLen + 1];

	//Convert
	::MultiByteToWideChar(CP_UTF8, NULL, strUtf8.c_str(), strUtf8.length(), pwcsUniString, nUniLen);
	pwcsUniString[nUniLen] = '\0';

	wstring wstrUnicode = pwcsUniString;

	return wstrUnicode;
}

//************************************
// Method:    Convert Unicode string to Ansi string
// FullName:  _UnicodeToAnsi
// Returns:   string
// Qualifier: protected
// Parameter: tstring
//				tstring Unicode
//************************************
string PublicFunc::_UnicodeToAnsi(wstring wstrUnicode)
{
	//Check the len
	if (wstrUnicode.empty())
	{
		return string();
	}

	//Wide char to multi bytes
	int nAnsiLen = ::WideCharToMultiByte(CP_ACP, NULL, wstrUnicode.c_str(), wstrUnicode.length(), NULL, 0, NULL, NULL);
	char* pMultiByteChar = new char[nAnsiLen + 1];
	memset(pMultiByteChar, 0, nAnsiLen + 1);

	::WideCharToMultiByte(CP_ACP, NULL, wstrUnicode.c_str(), wstrUnicode.length(), pMultiByteChar, nAnsiLen, NULL, NULL);

	string strRet = pMultiByteChar;

	delete pMultiByteChar;

	return strRet;
}

//************************************
// Method:    Convert Ansi string to Unicode
// FullName:  _AnsiToUnicode
// Returns:   tstring
// Qualifier: protected
// Parameter: string
//				string Ansi
//************************************
wstring PublicFunc::_AnsiToUnicode(string strAnsi)
{
	//Check the len
	if (strAnsi.empty())
	{
		return wstring();
	}

	//Multi bytes to wide char
	int nUniLen = ::MultiByteToWideChar(CP_ACP, NULL, strAnsi.c_str(), strAnsi.length(), NULL, 0);
	WCHAR* pWideChar = new WCHAR[nUniLen + 1];
	memset(pWideChar, 0, nUniLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, NULL, strAnsi.c_str(), strAnsi.length(), pWideChar, nUniLen);

	wstring wstrRet = pWideChar;

	delete pWideChar;

	return wstrRet;
}


unsigned char ToHex(unsigned char x) 
{ 
	return  x > 9 ? x + 55 : x + 48; 
}

unsigned char FromHex(unsigned char x) 
{ 
	unsigned char y;
	if (x >= 'A' && x <= 'Z') 
	{
		y = x - 'A' + 10;
	}
	else if (x >= 'a' && x <= 'z')
	{
		y = x - 'a' + 10;
	}
	else if (x >= '0' && x <= '9')
	{
		y = x - '0';
	}
	else assert(0);

	return y;
}

string PublicFunc::UrlEncode(const string& str)
{
	string strTemp = "";
	size_t length = str.length();

	for (size_t i = 0; i < length; i++)
	{
		if ( isalnum((unsigned char)str[i]) || (str[i] == '-') || (str[i] == '_') || (str[i] == '.') || (str[i] == '~') )
		{
			strTemp += str[i];
		}
		else if (str[i] == ' ')
		{
			strTemp += "+";
		}
		else
		{
			strTemp += '%';
			strTemp += ToHex((unsigned char)str[i] >> 4);
			strTemp += ToHex((unsigned char)str[i] % 16);
		}
	}

	return strTemp;
}

string PublicFunc::UrlDecode(const string& str)
{
	string strTemp = "";
	size_t length = str.length();

	for (size_t i = 0; i < length; i++)
	{
		if (str[i] == '+')
		{
			strTemp += ' ';
		}
		else if (str[i] == '%')
		{
			assert(i + 2 < length);
			unsigned char high = FromHex((unsigned char)str[++i]);
			unsigned char low = FromHex((unsigned char)str[++i]);
			strTemp += high * 16 + low;
		}
		else strTemp += str[i];
	}
	return strTemp;
}

//************************************
// Method:    Convert full phone number to non-international district number
// FullName:  FullNumberToShortNumber
// Returns:   tstring
// Qualifier: public
// Parameter: tstring
//				Full number, eg:008618666665555 or +8618666665555
//************************************
tstring PublicFunc::FullNumberToShortNumber(tstring wstrFullNumber)
{
	tstring tempLegalStr;

	//Delete the nonlegal alpha
	for (unsigned int nStrIndex = 0; nStrIndex < wstrFullNumber.length(); ++nStrIndex)
	{
		if (wstrFullNumber[nStrIndex] == L'+' && nStrIndex == 0)
		{
			tempLegalStr += _T("00");
		}
		else if (wstrFullNumber[nStrIndex] < 0x0030 || wstrFullNumber[nStrIndex] > 0x0039)
		{
			continue;
		}
		else
		{
			tempLegalStr += wstrFullNumber[nStrIndex];
		}
	}

	wstrFullNumber = tempLegalStr;

	tstring tempStr;
	tempStr.assign(wstrFullNumber.c_str(), 3);

	if (tempStr == _T("861") && wstrFullNumber.length() == 13)
	{
		wstrFullNumber.assign(wstrFullNumber, 2, wstrFullNumber.length());
	}
	else if (tempStr == _T("+86") || tempStr == _T(" 86"))
	{
		wstrFullNumber.assign(wstrFullNumber, 3, wstrFullNumber.length());
	}

	tempStr.assign(wstrFullNumber.c_str(), 4);

	if (tempStr == _T("0086"))
	{
		wstrFullNumber.assign(wstrFullNumber, 4, wstrFullNumber.length());
	}

	tempStr.assign(wstrFullNumber.c_str(), 5);
	// 17951/17911/12593
	if (tempStr == _T("17951") || tempStr == _T("17911") || tempStr == _T("12593") || tempStr == _T("17901") || tempStr == _T("17909"))
	{
		wstrFullNumber.assign(wstrFullNumber, 5, wstrFullNumber.length());
	}
	
	return wstrFullNumber;
}

//************************************
// Method:    Convert non-international district number to full phone number
// FullName:  ShortNumberToFullNumber
// Returns:   tstring
// Qualifier: public
// Parameter: tstring
//				Short number, eg:18666665555
//************************************
tstring PublicFunc::ShortNumberToFullNumber(tstring wstrShortNumber)
{
	tstring tempLegalStr;

	//Delete the nonlegal alpha
	for (unsigned int nStrIndex = 0; nStrIndex < wstrShortNumber.length(); ++nStrIndex)
	{
		if (wstrShortNumber[nStrIndex] == '+' && nStrIndex == 0)
		{
			tempLegalStr += _T("00");
		}
		else if (wstrShortNumber[nStrIndex] < 0x0030 || wstrShortNumber[nStrIndex] > 0x0039)
		{
			continue;
		}
		else
		{
			tempLegalStr += wstrShortNumber[nStrIndex];
		}
	}

	wstrShortNumber = tempLegalStr;

	tstring wstrTempHeader = tstring(wstrShortNumber, 0, 2);
	tstring wstrTempHeaderThree = tstring(wstrShortNumber, 0, 3);

	if (wstrTempHeader == _T("00"))
	{
		return wstrShortNumber;
	}
	else if (wstrTempHeader == _T("86") && wstrShortNumber.length() == 13)
	{
		wstrShortNumber = _T("00") + wstrShortNumber;
		return wstrShortNumber;
	}
	else if (wstrTempHeaderThree == _T("+86"))
	{
		wstrShortNumber = _T("0086") + tstring(wstrShortNumber, 3, wstrShortNumber.length());
		return wstrShortNumber;
	}

	if (wstrTempHeader[0] == '0' || wstrTempHeader[0] == '1')
	{
		wstrShortNumber = _T("0086") + wstrShortNumber;

		return wstrShortNumber;
	}
	else
	{
		/*
		LocationFinder* locFinder = LocationFinder::GetLocationFinder();

		tstring wstrChineseLocCode;
		locFinder->FindChineseLocCode(wstrShortNumber, wstrChineseLocCode);

		wstrShortNumber = L"0086" + wstrChineseLocCode + wstrShortNumber;
		*/

		return wstrShortNumber;
	}
}

//Remove same number
vector<tstring> PublicFunc::GetAllNumberNoRepeat(vector<tstring> vecAllNumber)
{
	vector<tstring> vecRet;

	map<tstring, bool> mapNumber;
	for (int nIndex = 0; nIndex < vecAllNumber.size(); ++nIndex)
	{
		mapNumber[FullNumberToShortNumber(vecAllNumber[nIndex])] = true;
	}

	map<tstring, bool>::iterator iterMap = mapNumber.begin();
	while (iterMap != mapNumber.end())
	{
		vecRet.push_back(iterMap->first);
		++iterMap;
	}

	return vecRet;
}


void PublicFunc::StringReplace(string &strBase, const string &strOld, const string &strNew)  
{  
	string::size_type pos = 0;  
	string::size_type srcLen = strOld.size();  
	string::size_type desLen = strNew.size();  
	pos = strBase.find(strOld, pos);   
	while ((pos != string::npos))  
	{  
		strBase.replace(pos, srcLen, strNew);  
		pos = strBase.find(strOld, (pos+desLen));  
	}  
}



// 判断是否是中国的手机号码
BOOL PublicFunc::IsChineseMobileNumber(const tstring strNumber)
{
	BOOL bRet = FALSE;

	tstring strTemp = strNumber;
	replace(strTemp.begin(), strTemp.end(), '-', ' ');

	tstring wstrTemp;
	int nPos = strTemp.length();
	while (nPos < strTemp.length())
	{
		if (strTemp[nPos] == '+')
		{
			wstrTemp += _T("00");
		}
		else
		{
			wstrTemp += strTemp[nPos];
		}
	}
	
	if (-1 != strTemp.find(_T("0086")))
	{
		strTemp = strTemp.substr(strTemp.length() - 4, strTemp.length());
	}

	if (11 == strTemp.length() && '1' == strTemp[0])
	{
		bRet = TRUE;
	}

	return bRet;
}


// 获得程序运行的系统及版本
bool PublicFunc::GetOSTypeAndVersion(tstring& wstrOSType, tstring& wstrOSVersion)
{
	HKEY hKey;
	DWORD type;
	DWORD dwSize;
	LONG ReturnValue;
	TCHAR wstrBuf[100];

	//Init OS info
	wstrOSType = _T("Windows");
	wstrOSVersion = _T("unknown version");

	//Open regedit
	ReturnValue = RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
		_T("SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion"), 
		0, KEY_ALL_ACCESS, &hKey);

	if(ERROR_SUCCESS != ReturnValue)
	{
		return false;
	}

	//Get OS type name
	LONG lReturn = RegQueryValueEx (hKey, _T("ProductName"), NULL, &type, (BYTE *)wstrBuf, &dwSize);

	if(ERROR_SUCCESS != lReturn)
	{
		return false;
	}

	wstrOSType = wstrBuf;
	memset(wstrBuf, 0, 200);

	//Get OS version
	lReturn = RegQueryValueEx (hKey, _T("CurrentVersion"), NULL, &type, (BYTE *)wstrBuf, &dwSize);

	if(ERROR_SUCCESS != lReturn)
	{
		return false;
	}

	wstrOSVersion = wstrBuf;

	return true;
}

int PublicFunc::GetMacAddress(vector<tstring> &LanMAC)  
{ 
#define MAX_ADAPTER_NAME_LENGTH 256
#define MAX_ADAPTER_DESCRIPTION_LENGTH 128
#define MAX_ADAPTER_ADDRESS_LENGTH 8
	typedef struct _NET_IP_ADDRESS_STRING{
		char String[16];
	} NET_IP_ADDRESS_STRING, *PNET_IP_ADDRESS_STRING, NET_IP_MASK_STRING, *PNET_IP_MASK_STRING;
	typedef struct _NET_IP_ADDR_STRING {
		struct _NET_IP_ADDR_STRING* Next;
		NET_IP_ADDRESS_STRING IpAddress;
		NET_IP_MASK_STRING IpMask;
		DWORD Context;
	} NET_IP_ADDR_STRING, *PNET_IP_ADDR_STRING;
	typedef struct _NET_IP_ADAPTER_INFO {
		struct _NET_IP_ADAPTER_INFO* Next;
		DWORD ComboIndex;
		char AdapterName[MAX_ADAPTER_NAME_LENGTH+4];
		char Description[MAX_ADAPTER_DESCRIPTION_LENGTH+4];
		UINT AddressLength;
		BYTE Address[MAX_ADAPTER_ADDRESS_LENGTH];
		DWORD Index;
		UINT Type;
		UINT DhcpEnabled;
		PNET_IP_ADDR_STRING CurrentIpAddress;
		NET_IP_ADDR_STRING IpAddressList;
		NET_IP_ADDR_STRING GatewayList;
		NET_IP_ADDR_STRING DhcpServer;
		BOOL HaveWins;
		NET_IP_ADDR_STRING PrimaryWinsServer;
		NET_IP_ADDR_STRING SecondaryWinsServer;
		time_t LeaseObtained;
		time_t LeaseExpires;
	} NET_IP_ADAPTER_INFO, *PNET_IP_ADAPTER_INFO;

	LanMAC.clear();

	HINSTANCE hLib = 0;
	if ( hLib = LoadLibrary(_T( "iphlpapi.dll") ) ) //加载动态库
	{
		DWORD  (WINAPI*pGetAdaptersInfo ) (PNET_IP_ADAPTER_INFO,PULONG) =
			( DWORD (WINAPI*) (PNET_IP_ADAPTER_INFO,PULONG) )GetProcAddress(hLib, "GetAdaptersInfo");//获取地址
		if ( pGetAdaptersInfo != 0  )
		{
			DWORD AdapterInfoSize = 0;
			if( pGetAdaptersInfo(0, &AdapterInfoSize) == ERROR_BUFFER_OVERFLOW) //获取长度
			{
				PNET_IP_ADAPTER_INFO pAdapterInfo = (PNET_IP_ADAPTER_INFO) new BYTE [AdapterInfoSize]; //分配内存

				if ( pGetAdaptersInfo(pAdapterInfo,   &AdapterInfoSize) == ERROR_SUCCESS) //获取信息
				{
					PNET_IP_ADAPTER_INFO pAdapter = pAdapterInfo;
					while(pAdapter)
					{
						tstring strMac;
						TCHAR wcMac[32] = {0};
						sprintf(wcMac, "%02X-%02X-%02X-%02X-%02X-%02X",
							pAdapter->Address[0],pAdapter->Address[1],pAdapter->Address[2],
							pAdapter->Address[3],pAdapter->Address[4],pAdapter->Address[5]);

						strMac = wcMac;

						LanMAC.push_back(strMac);

						pAdapter = pAdapter->Next; //下一个区域
					}
				}

				delete [] pAdapterInfo; //释放内存
			}
		}

		FreeLibrary( hLib ); //释放库
	}

	if (LanMAC.empty())
	{
		LanMAC.push_back("00-00-00-00-00-00");
	}

	return LanMAC.size(); //返回个数
}

//转换字符串为大写
tstring PublicFunc::ToUppercase(const tstring& wstrSource)
{
	tstring wstrOut = wstrSource;

	for (int nIndex = 0; nIndex != wstrSource.size(); ++nIndex)
	{
		if (wstrSource[nIndex] >= 'a' && wstrSource[nIndex] <= 'z')
		{
			wstrOut[nIndex] = wstrSource[nIndex] - ('a' - 'A');
		}
	}

	return wstrOut;
}


void PublicFunc::Trim(tstring &src)
{
	//string str = _UnicodeToAnsi(src);
	//str.erase(str.begin(), std::find_if(str.begin(), str.end(),	std::not1(std::ptr_fun(::isspace))));
	//str.erase(std::find_if(str.rbegin(), str.rend(), std::not1(std::ptr_fun(::isspace))).base(), str.end());	

	//src = _AnsiToUnicode(str);
}

// 通过资源获取资源指针
LPVOID PublicFunc::GetResMemory(UINT IDResource, const tstring &strType, DWORD &dwSize)
{
	// 定位我们的自定义资源，这里因为我们是从本模块定位资源，所以将句柄简单地置为NULL即可
	HRSRC hRsrc = FindResource(NULL, MAKEINTRESOURCE(IDResource), strType.c_str());
	if (NULL == hRsrc)
		return NULL;
	// 获取资源的大小
	dwSize = SizeofResource(NULL, hRsrc); 
	if (0 == dwSize)
		return NULL;

	// 加载资源
	HGLOBAL hGlobal = LoadResource(NULL, hRsrc); 
	if (NULL == hGlobal)
		return NULL;

	// 锁定资源
	LPVOID pBuffer = LockResource(hGlobal); 

	return pBuffer;
}

void PublicFunc::TimeFormat(__int64 nMsgTime, tstring &strTime, bool bShowDetail)
{	
	tm * pMessageTm = localtime(&nMsgTime);
	if ( NULL == pMessageTm ) return ;
	
	tm messageTime = *pMessageTm;

	time_t currentTime = time(NULL);
	tm* pCurrentTm = localtime(&currentTime);

	TCHAR wcTime[128] = {0};
	if ( messageTime.tm_year == pCurrentTm->tm_year && messageTime.tm_mon == pCurrentTm->tm_mon && messageTime.tm_mday == pCurrentTm->tm_mday )
	{// 当天
		strftime(wcTime, 128, "%#H:%M", &messageTime);
	}
	else if (messageTime.tm_year == pCurrentTm->tm_year && messageTime.tm_mon == pCurrentTm->tm_mon && messageTime.tm_mday != pCurrentTm->tm_mday)
	{// 当月
		if ( pCurrentTm->tm_mday - messageTime.tm_mday > 1 )
		{//
			if ( bShowDetail )
			{
				strftime(wcTime, 128, "%#m-%#d %#H:%M", &messageTime);
			}
			else
			{
				strftime(wcTime, 128, "%#m-%#d", &messageTime);
			}
		}
		else
		{
			strTime = "昨天 ";
			if ( bShowDetail )
			{
				strftime(wcTime, 128, "%#H:%M", &messageTime);
			}			
		}		
	}
	else if ( messageTime.tm_year == pCurrentTm->tm_year )
	{// 同年
		if ( bShowDetail )
		{
			strftime(wcTime, 128,"%#m-%#d %#H:%M", &messageTime);
		}
		else
		{
			strftime(wcTime, 128, "%#m-%#d", &messageTime);
		}
	}
	else if (messageTime.tm_year != pCurrentTm->tm_year)
	{// 跨年
		strftime(wcTime, 128, "%#Y-%#m-%#d", &messageTime);
	}
	else
	{
		strftime(wcTime, 128, "%#Y-%#m-%#d  %#H:%M:%S", &messageTime);
	}

	strTime += wcTime;	
}

DWORD PublicFunc::GetFileSize(const tstring & strFilePath)
{
	DWORD lowSize, highSize;
	HANDLE hFile;

	hFile = CreateFile(strFilePath.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE) 
	{		
		return 0;
	}

	lowSize = ::GetFileSize(hFile, &highSize);
	CloseHandle(hFile);

	return lowSize;
}

tstring PublicFunc::Uint64ToString(const unsigned __int64 nNumber)
{	
	TCHAR wcId[64] = {0};
	sprintf(wcId, "%llu", nNumber);

	return tstring(wcId);
}

unsigned __int64 PublicFunc::StringToUint64(const tstring wstrNumber)
{
	return static_cast<unsigned __int64>(_atoi64(wstrNumber.c_str()));
}