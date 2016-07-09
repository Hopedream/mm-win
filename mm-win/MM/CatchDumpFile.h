#pragma once

#include "string"
#include "imagehlp.h"
#pragma comment( lib, "..\\lib\\DbgHelp.lib")
#pragma comment( lib, "version.lib")
namespace CatchDumpFile 
{

	void simple_log(const std::tstring& log_msg);

	class CDumpCatch
	{
	public:
		CDumpCatch();
		~CDumpCatch();
	private:

		static LPTOP_LEVEL_EXCEPTION_FILTER WINAPI TempSetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);
		static BOOL ReleaseDumpFile(const std::tstring& strPath, EXCEPTION_POINTERS *pException);
		static LONG WINAPI UnhandledExceptionFilterEx(struct _EXCEPTION_POINTERS *pException);
		static void MyPureCallHandler(void);
		static void MyInvalidParameterHandler(const wchar_t* expression, const wchar_t* function, const wchar_t* file, unsigned int line, uintptr_t pReserved);


		BOOL AddExceptionHandle();
		BOOL RemoveExceptionHandle();
		BOOL PreventSetUnhandledExceptionFilter();
		void SetInvalidHandle();
		void UnSetInvalidHandle();
	private:
		LPTOP_LEVEL_EXCEPTION_FILTER m_preFilter;
		_invalid_parameter_handler m_preIph;
		_purecall_handler m_prePch;    
	};
};