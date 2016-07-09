#pragma once
#include "StdAfx.h"
#include "oss_interface.h"
#include "UIDefine.h"
#include <queue>

using namespace std;

class CFileControl
{
private:
	int m_iuploadTaskNum;                 //�ϴ���������
	int m_idownloadTaskNum;               //������������
	int m_iCurdownloadTaskNum;            //��ǰ�������������
	int m_iCuruploadTaskNum;              //��ǰ�ϴ����������
	vector<fileInfo> m_lstupLoadFiles;     //��ǰ�ϴ������б�
	vector<fileInfo> m_lstdownLoadFiles;     //��ǰ���������б�
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

	HANDLE upload_handle_;//�ϴ��߳�
	unsigned upload_handle_id_;
	queue<sFileInfo*> upload_file_info_queue_;//�ϴ��ļ�����
	CRITICAL_SECTION upload_queue_cs_;//�����漰�����߳�д��ɾ������

	HANDLE download_handle_;//�����߳�
	unsigned download_handle_id_;
	queue<sFileInfo*> download_file_info_queue_;//�����ļ�����
	CRITICAL_SECTION download_queue_cs_;//�����漰�����߳�д��ɾ������


	//������Ҫ���˳����߳������˳�
	bool request_exit_;
};

