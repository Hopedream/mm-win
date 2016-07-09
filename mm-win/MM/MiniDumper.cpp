#include "stdafx.h"
#include "MiniDumper.h"
#include "FileHelper.h"
#include "MsgBox.h"

BOOL WriteMiniDump(PEXCEPTION_POINTERS excpInfo, CDuiString strFileName)
{
	MINIDUMP_EXCEPTION_INFORMATION eInfo;
	CDuiString strDumpFilePath = (_T(""));
	//TCHAR szTemp[1024] = {0};
	BOOL bRet = FALSE;
	HANDLE hFile = NULL;

	try
	{
		if (strFileName.IsEmpty())
		{
			SYSTEMTIME systemTime;
			GetLocalTime(&systemTime);
			strFileName.Format(_T("SWDownloader%s_Minidump_%d%d%d%d%d%d.dmp"), CFileHelper::GetCurrentAppPath(),
								systemTime.wYear,systemTime.wMonth,systemTime.wDay,
								systemTime.wHour,systemTime.wMinute,systemTime.wSecond);	

		}
		CDuiString strDumpFilePath = CFileHelper::GetCurrentAppPath();
		strDumpFilePath = strDumpFilePath + strFileName;


		hFile = CreateFile( strDumpFilePath.GetData(), GENERIC_READ | GENERIC_WRITE, 
			0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); 
		
		eInfo.ThreadId = GetCurrentThreadId();
		eInfo.ExceptionPointers = excpInfo;
		eInfo.ClientPointers = FALSE;

		bRet = MiniDumpWriteDump(
			GetCurrentProcess(),
			GetCurrentProcessId(),
			hFile,
			MiniDumpWithFullMemory,
			&eInfo,
			NULL,
			NULL);
	}
	catch (...)
	{
		//MessageBox(_T("Catch a fatal error when dump the minidump!"));
	}

	CloseHandle( hFile );
	return bRet;		
}

LONG MiniDump_Handled( PEXCEPTION_POINTERS excpInfo,const TCHAR  *szErrorMsg )
{
	BOOL bRet = WriteMiniDump(excpInfo);
	CDuiString strMsg;
	if (bRet)
	{		
		strMsg.Format(_T("MM Exception Error:%s "),szErrorMsg);
	}
	else
	{
		DWORD dwErr = GetLastError();
		strMsg.Format( _T("SWDownloader encounter a fatal error in SWDownloader_MiniDump_Handled(), will exit! Write MiniDump file Fail! ErrCode: %d"), dwErr);
	}

	CMsgBox* pMsgBox = new CMsgBox("", strMsg.GetData());
	pMsgBox->ShowModal();

	return EXCEPTION_EXECUTE_HANDLER;
}

 LONG MiniDump_Unhandled( PEXCEPTION_POINTERS excpInfo )
{
	CDuiString strErr;
	CDuiString strDumpFileName;

	strDumpFileName = _T("MM");
	strDumpFileName += GetCurrentAppVer();
	strDumpFileName += _T("_Minidump.dmp");
	CDuiString strErrMsg = (_T(""));
	try
	{
		BOOL bRet = WriteMiniDump(excpInfo, strDumpFileName);
		if (bRet)
		{
			CDuiString strDmpPath = (_T(""));			
			// Get DebugLog folder path.
			strDmpPath= CFileHelper::GetCurrentAppPath();	
			strDmpPath += strDumpFileName;
			// write the mail body
			strErrMsg = _T("The dump file path: ");
			strErrMsg += strDmpPath;
			strErrMsg += _T("\nPlease Send the dump file to us to help us improve the SWDownloader. Thank you!");
		}
		else
		{
			DWORD dwErr = GetLastError();
			strErrMsg.Format( _T("SWDownloader encounter a fatal error in SWDownloader_MiniDump_Unhandled(), will exit! Write MiniDump file Fail! ErrCode: %d"), dwErr);
		}
	}
	catch(...)
	{
		strErrMsg = _T("Exception in SWDownloader_MiniDump_Unhandled(), it a fatal error!");
	}
	
	CMsgBox* pMsgBox = new CMsgBox("", strErrMsg.GetData());
	pMsgBox->ShowModal();

	return EXCEPTION_EXECUTE_HANDLER;
}

CDuiString GetCurrentAppVer()
{
	DWORD dummy;
	TCHAR szTemp[1024];
	CDuiString strVersion = (_T(""));
	::GetModuleFileName(NULL, szTemp, 1024);
	DWORD dwSize = GetFileVersionInfoSize(szTemp, &dummy);
	if (dwSize != 0)
	{
		std::vector<BYTE> data(dwSize);
		if (GetFileVersionInfo(szTemp, NULL, dwSize, &data[0]))
		{
			LPVOID pvProductName = NULL;
			unsigned int iProductNameLen = 0;
			LPVOID pvProductVersion = NULL;
			unsigned int iProductVersionLen = 0;

			// replace "040904e4" with the language ID of your resources
			if(! (!VerQueryValue(&data[0], _T("\\StringFileInfo\\040904e4\\ProductName"), &pvProductName, &iProductNameLen) ||
				!VerQueryValue(&data[0], _T("\\StringFileInfo\\040904e4\\ProductVersion"), &pvProductVersion, &iProductVersionLen)))
			{
				//strVersion.SetString((LPCTSTR)pvProductName, iProductNameLen);
				strVersion.Assign((LPCTSTR)pvProductVersion,iProductVersionLen);
			}

		}
	}

	return strVersion;
}