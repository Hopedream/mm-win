#include "stdafx.h"
#include "CatchDumpFile.h"

namespace CatchDumpFile
{
	void simple_log(const std::tstring& log_msg)
	{
		std::tstring strLogWnd = "cswuyg_simple_debug_log";
		HWND hSend = ::FindWindow(NULL, strLogWnd.c_str());
		COPYDATASTRUCT copydate;
		copydate.cbData = (DWORD)(log_msg.length() + 1) * sizeof(wchar_t);
		copydate.lpData = (PVOID)log_msg.c_str();
		::SendMessage(hSend, WM_COPYDATA, 0, (LPARAM)&copydate);
	}

	void CDumpCatch::MyPureCallHandler(void)
	{    
		//simple_log(L"MyPureCallHandler");
		throw std::invalid_argument(_T(""));
	}

	void CDumpCatch::MyInvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved)
	{
		//simple_log(L"MyPureCallHandler");
		//The parameters all have the value NULL unless a debug version of the CRT library is used.
		throw std::invalid_argument(_T(""));
	}

	void CDumpCatch::SetInvalidHandle()
	{
#if _MSC_VER >= 1400  // MSVC 2005/8
		m_preIph = _set_invalid_parameter_handler(MyInvalidParameterHandler);
#endif  // _MSC_VER >= 1400
		m_prePch = _set_purecall_handler(MyPureCallHandler);   //At application, this call can stop show the error message box.
	}
	void CDumpCatch::UnSetInvalidHandle()
	{
#if _MSC_VER >= 1400  // MSVC 2005/8
		_set_invalid_parameter_handler(m_preIph);
#endif  // _MSC_VER >= 1400
		_set_purecall_handler(m_prePch); //At application this can stop show the error message box.
	}

	LPTOP_LEVEL_EXCEPTION_FILTER WINAPI CDumpCatch::TempSetUnhandledExceptionFilter( LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter )
	{
		return NULL;
	}

	BOOL CDumpCatch::AddExceptionHandle()
	{
		m_preFilter = ::SetUnhandledExceptionFilter(UnhandledExceptionFilterEx);
		PreventSetUnhandledExceptionFilter();
		return TRUE;
	}

	BOOL CDumpCatch::RemoveExceptionHandle()
	{
		if(m_preFilter != NULL)
		{
			::SetUnhandledExceptionFilter(m_preFilter);
			m_preFilter = NULL;
		}
		return TRUE;
	}

	CDumpCatch::CDumpCatch()
	{
		SetInvalidHandle();
		AddExceptionHandle();
	}

	CDumpCatch::~CDumpCatch()
	{
		UnSetInvalidHandle();
		RemoveExceptionHandle();
	}

	BOOL CDumpCatch::ReleaseDumpFile(const std::tstring& strPath, EXCEPTION_POINTERS *pException)
	{
		HANDLE hDumpFile = ::CreateFile(strPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);  
		if (hDumpFile == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}
		MINIDUMP_EXCEPTION_INFORMATION dumpInfo;  
		dumpInfo.ExceptionPointers = pException;  
		dumpInfo.ThreadId = ::GetCurrentThreadId();  
		dumpInfo.ClientPointers = TRUE;  
		//    ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hDumpFile, MiniDumpNormal, &dumpInfo, NULL, NULL);  
		BOOL bRet = ::MiniDumpWriteDump(::GetCurrentProcess(), ::GetCurrentProcessId(), hDumpFile, MiniDumpWithFullMemory, &dumpInfo, NULL, NULL);  
		::CloseHandle(hDumpFile);  
		return bRet;
	}

	LONG WINAPI CDumpCatch::UnhandledExceptionFilterEx( struct _EXCEPTION_POINTERS *pException )
	{
		//simple_log(L"UnhandledExceptionFilterEx");
		char szPath[MAX_PATH] = { 0 };
		::GetModuleFileName(NULL, szPath, MAX_PATH);
		::PathRemoveFileSpec(szPath);
		std::tstring strPath = szPath;
		strPath += "\\CrashDumpFile.dmp";
		BOOL bRelease = ReleaseDumpFile(strPath.c_str(), pException);
		//::FatalAppExit(0,  L"Error");
		if (bRelease)
		{
			return EXCEPTION_EXECUTE_HANDLER;
		}
		return EXCEPTION_CONTINUE_SEARCH;
	}

	BOOL CDumpCatch::PreventSetUnhandledExceptionFilter()
	{
		HMODULE hKernel32 = LoadLibrary("kernel32.dll");
		if (hKernel32 ==   NULL)
		{
			return FALSE;
		}
		void *pOrgEntry = ::GetProcAddress(hKernel32, "SetUnhandledExceptionFilter");
		if(pOrgEntry == NULL)
		{
			return FALSE;
		}

		unsigned char newJump[5];
		DWORD dwOrgEntryAddr = (DWORD)pOrgEntry;
		dwOrgEntryAddr += 5; //jump instruction has 5 byte space.

		void *pNewFunc = &TempSetUnhandledExceptionFilter;
		DWORD dwNewEntryAddr = (DWORD)pNewFunc;
		DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;

		newJump[0] = 0xE9;  //jump
		memcpy(&newJump[1], &dwRelativeAddr, sizeof(DWORD));
		SIZE_T bytesWritten;
		DWORD dwOldFlag, dwTempFlag;
		::VirtualProtect(pOrgEntry, 5, PAGE_READWRITE, &dwOldFlag);
		BOOL bRet = ::WriteProcessMemory(::GetCurrentProcess(), pOrgEntry, newJump, 5, &bytesWritten);
		::VirtualProtect(pOrgEntry, 5, dwOldFlag, &dwTempFlag);
		return bRet;
	}

}