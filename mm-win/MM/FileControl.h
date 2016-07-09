#pragma once
#include "StdAfx.h"
#include "oss_interface.h"
#include "UIDefine.h"
#include <queue>

using namespace std;

class CFileControl
{
private:
	int m_iuploadTaskNum;                 //上传任务总数
	int m_idownloadTaskNum;               //下载任务总数
	int m_iCurdownloadTaskNum;            //当前下载完成任务数
	int m_iCuruploadTaskNum;              //当前上传完成任务数
	vector<fileInfo> m_lstupLoadFiles;     //当前上传任务列表
	vector<fileInfo> m_lstdownLoadFiles;     //当前下载任务列表
public:
	CFileControl(void);
	~CFileControl(void);
	bool uploadFiles(fileInfo& lstFiles, IOssEventBase* pEventHandle);
	bool downloadFiles(fileInfo& lstFiles, IOssEventBase* pEventHandle);
	bool downloadSignalFile(fileInfo& infoFile, IOssEventBase* pEventHandle);
private:
	static unsigned  __stdcall  downloadThreadFunc( void*pArguments);
	static unsigned  __stdcall  uploadThreadFunc( void*pArguments); 
	bool uploadSignalFile(fileInfo infoFile, IOssEventBase* pEventHandle);
	//bool downloadSignalFile(fileInfo& infoFile, IOssEventBase* pEventHandle);

	struct sFileInfo
	{
		fileInfo pFile;
		IOssEventBase* pEventHandle;
		sFileInfo()
		{
			pEventHandle=NULL;
		}
	};

	HANDLE upload_handle_;//上传线程
	unsigned upload_handle_id_;
	queue<sFileInfo*> upload_file_info_queue_;//上传文件队列
	CRITICAL_SECTION upload_queue_cs_;//队列涉及到多线程写，删，加锁

	HANDLE download_handle_;//下载线程
	unsigned download_handle_id_;
	queue<sFileInfo*> download_file_info_queue_;//下载文件队列
	CRITICAL_SECTION download_queue_cs_;//队列涉及到多线程写，删，加锁


	//主进程要求退出，线程自行退出
	bool request_exit_;
};

