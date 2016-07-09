#include "..\MM\StdAfx.h"
#include "Logger.h"
#include "../MM/FileHelper.h"


CLogger* CLogger::m_pInstance = NULL;

CLogger::CLogger()
{
	m_eLogType = LOG_ERROR;	
}

CLogger::~CLogger()
{
	CloseLogFile();
}

void CLogger::Init(ENMLOGTYPE eLogType)
{
	m_eLogType = eLogType;
	DuiLib::CDuiString strLogFile;
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	CDuiString strLogPath = CFileHelper::GetUserDir() + "Log";
	if (CFileHelper::CreateFolder(strLogPath.GetData()))
	{
		strLogFile.Format("%s\\%d_%d_%d.log", 
			strLogPath.GetData(), 
			sysTime.wYear, sysTime.wMonth, sysTime.wDay);

		m_logStream.open(strLogFile.GetData(), ios::app);
	}	
}

void CLogger::CloseLogFile()
{
	if(m_logStream.is_open())
	{
		m_logStream.close();
	}
}

void CLogger::PrintLog(LPCTSTR lpszLog, ENMLOGTYPE eLogType)
{
	if(lpszLog && eLogType >= m_eLogType)
	{
		TCHAR szType[10] = {0};
		switch (eLogType)
		{
		case LOG_ERROR:
			_tcscpy(szType, _T("Error"));
			break;
		case LOG_INFO:
			_tcscpy(szType, _T("Info"));
			break;
		case LOG_DEBUG:
			_tcscpy(szType, _T("Verbose"));
			break;
		}

		if(m_logStream.good())
		{
			SYSTEMTIME sysTime;
			GetLocalTime(&sysTime);
			m_logStream << "[" << sysTime.wHour <<  ":" 
									<< sysTime.wMinute <<  ":"  
									<< sysTime.wSecond << ":" 
									<< sysTime.wMilliseconds << "]  "
									<< lpszLog << endl;
		}
		else
		{
			::OutputDebugString(szType);
			::OutputDebugString(lpszLog);
			::OutputDebugString(_T("\n"));
		}
	}
}

void CLogger::PrintLog( ENMLOGTYPE eLogType, const TCHAR* _Format,... )
{
	TCHAR szBuffer[MAX_LOG_LEN];
	va_list argptr; 		
	va_start(argptr, _Format); 
	_vsntprintf(szBuffer,MAX_LOG_LEN,_Format,argptr);  
	va_end(argptr); 	
	PrintLog(szBuffer, eLogType);
}

CLogger* CLogger::GetInstance()
{
	if(m_pInstance == NULL)
	{
		m_pInstance = new CLogger();
	}
	return m_pInstance;
}

void CLogger::PrintErrLog( const TCHAR* _Format,... )
{
	TCHAR szBuffer[MAX_LOG_LEN];
	va_list argptr; 		
	va_start(argptr, _Format); 
	_vsntprintf(szBuffer,MAX_LOG_LEN,_Format,argptr);  
	va_end(argptr); 	
	PrintLog(szBuffer, LOG_ERROR);
}

void CLogger::DestroyLogger()
{
	if (NULL != m_pInstance)
	{
		delete m_pInstance;
	}
}
