#pragma once
#include <stdio.h>
#include "TCHAR.h"
#include <fstream>
using namespace std;

#define MAX_LOG_LEN 1024
// Log Type
typedef enum _tagLogType
{
	LOG_ERROR = 0,
	LOG_WARN,
	LOG_INFO,
	LOG_DEBUG,
} ENMLOGTYPE;

class CLogger
{
public:
	static CLogger* GetInstance();
	void Init(ENMLOGTYPE eLogType = LOG_ERROR);
	void PrintLog(LPCTSTR lpszlog, ENMLOGTYPE eLogType);
	void PrintLog( ENMLOGTYPE eLogType, const TCHAR* _Format,...);
	void PrintErrLog(const TCHAR* _Format,...);

	void DestroyLogger();

private:
	CLogger();
	CLogger(const CLogger&);
	CLogger &operator= (const CLogger&);
	~CLogger();
	void CloseLogFile();

private:
	static CLogger* m_pInstance;
	ENMLOGTYPE m_eLogType;
	//FILE* m_pLogFile;
	ofstream m_logStream;
};

#ifdef USELOG
	#define LOG_ERROR(s)   CLogger::GetInstance()->PrintLog(s, LOG_ERROR)
	#define LOG_INFO(s)    CLogger::GetInstance()->PrintLog(s, LOG_INFO)
	#define LOG_DEBUG(s) CLogger::GetInstance()->PrintLog(s, LOG_DEBUG)
#else
	#define LOG_ERROR(s) 
	#define LOG_INFO(s)   
	#define LOG_DEBUG(s) 
#endif


