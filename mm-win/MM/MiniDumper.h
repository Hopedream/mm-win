#pragma once
//#include "dbghelp.h"
#include "imagehlp.h"




#pragma comment( lib, "..\\lib\\DbgHelp.lib")
#pragma comment( lib, "version.lib")

BOOL WriteMiniDump(PEXCEPTION_POINTERS excpInfo, CDuiString strFileName = (_T("")));
LONG MiniDump_Handled(PEXCEPTION_POINTERS excpInfo, const TCHAR *szMsg=(_T("")));
LONG MiniDump_Unhandled(PEXCEPTION_POINTERS excpInfo);
CDuiString GetCurrentAppVer();