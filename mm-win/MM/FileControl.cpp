#include "FileControl.h"
#include "Controller.h"
#include "mainDlg.h"
#include "UIDefine.h"
#include "FileHelper.h"
#include "StringHelper.h"
#include <atlstr.h>

CFileControl::CFileControl(void)
{
	m_iuploadTaskNum = 0;                 
	m_idownloadTaskNum = 0;               
	m_iCurdownloadTaskNum = 0;            
	m_iCuruploadTaskNum = 0;  

	InitializeCriticalSection(&upload_queue_cs_);
	InitializeCriticalSection(&download_queue_cs_);
	request_exit_ = false;
	upload_handle_ = (HANDLE)_beginthreadex(NULL, 0, uploadThreadFunc, this , 0, &upload_handle_id_);
	download_handle_ = (HANDLE)_beginthreadex(NULL, 0, downloadThreadFunc, this , 0, &download_handle_id_);
}


CFileControl::~CFileControl(void)
{
	request_exit_ = true;
	WaitForSingleObject(upload_handle_, INFINITE);
	WaitForSingleObject(download_handle_, INFINITE);
	DeleteCriticalSection(&upload_queue_cs_);
	DeleteCriticalSection(&download_queue_cs_);
}

unsigned  __stdcall CFileControl::downloadThreadFunc( void*pArguments )
{
	CFileControl* pThis = (CFileControl*)pArguments;
	while(true)
	{
		if (pThis->request_exit_)
		{
			break;
		}
		EnterCriticalSection(&pThis->download_queue_cs_);
		int size = pThis->download_file_info_queue_.size();
		LeaveCriticalSection(&pThis->download_queue_cs_);
		if ((NULL != pThis) &&  size != 0)
		{
			sFileInfo* temp = (pThis->download_file_info_queue_).front();

			pThis->downloadSignalFile(temp->pFile, temp->pEventHandle);

			EnterCriticalSection(&pThis->download_queue_cs_);
			pThis->download_file_info_queue_.pop();
			LeaveCriticalSection(&pThis->download_queue_cs_);
		}
		else
		{
			Sleep(100);
		}
	}
	return   0;  
}

unsigned  __stdcall CFileControl::uploadThreadFunc( void*pArguments )
{
	CFileControl* pThis = (CFileControl*)pArguments;
	while(true)
	{
		if (pThis->request_exit_)
		{
			break;
		}
		EnterCriticalSection(&pThis->upload_queue_cs_);
		int size = pThis->upload_file_info_queue_.size();
		LeaveCriticalSection(&pThis->upload_queue_cs_);
		if ((NULL != pThis) &&  size != 0)
		{
			sFileInfo* temp = (pThis->upload_file_info_queue_).front();

			pThis->uploadSignalFile(temp->pFile, temp->pEventHandle);

			EnterCriticalSection(&pThis->upload_queue_cs_);
			pThis->upload_file_info_queue_.pop();
			LeaveCriticalSection(&pThis->upload_queue_cs_);
		}
		else
		{
			Sleep(100);
		}
	}
	return   0;  
}

bool CFileControl::uploadFiles( fileInfo& lstFiles, IOssEventBase* pEventHandle)
{
	sFileInfo* pTmp = new sFileInfo;	
	pTmp->pFile.dwFileSize = lstFiles.dwFileSize;
	pTmp->pFile.strFileFullName = lstFiles.strFileFullName;
	pTmp->pFile.strFileName = lstFiles.strFileName;
	pTmp->pFile.strFilePath = lstFiles.strFilePath;
	pTmp->pFile.strFileSz = lstFiles.strFileSz;
	pTmp->pFile.strFileType = lstFiles.strFileType;
	pTmp->pFile.strFileUrl = lstFiles.strFileUrl;
	pTmp->pFile.strKey = lstFiles.strKey;
	pTmp->pFile.stropposite = lstFiles.stropposite;
	pTmp->pFile.strPackID = lstFiles.strPackID;
	pTmp->pFile.strPackName = lstFiles.strPackName;
	pTmp->pEventHandle = pEventHandle;

	EnterCriticalSection(&upload_queue_cs_);
	upload_file_info_queue_.push(pTmp);
	LeaveCriticalSection(&upload_queue_cs_);
	return true;
}

bool CFileControl::downloadFiles( fileInfo& lstFiles, IOssEventBase* pEventHandle)
{
	sFileInfo* pTmp = new sFileInfo;	
	pTmp->pFile = lstFiles;
	pTmp->pEventHandle = pEventHandle;

	EnterCriticalSection(&download_queue_cs_);
	download_file_info_queue_.push(pTmp);
	LeaveCriticalSection(&download_queue_cs_);
	return true;
}

bool CFileControl::uploadSignalFile( fileInfo infoFile, IOssEventBase* pEventHandle)
{
	//Init oss.
	//return false;  img-cn-hangzhou.aliyuncs.com
	//tstring strEndPoint("oss-cn-hangzhou.aliyuncs.com");
	tstring strEndPoint("img-cn-hangzhou.aliyuncs.com");
	tstring strAcessID("nHGOOiBnBsJCs04n");
	tstring strAcessIDSecret("Yod3tgIrcqVLpwuIk6yxRJKm56pvXr");
	tstring strBucket("jp-mm");

	ossInterface ossControl;
	ossControl.AddEventHandler(pEventHandle);
	ossControl.setAcessEndpoint(strEndPoint);
	ossControl.setAcessID(strAcessID);
	ossControl.setAcessIDSecret(strAcessIDSecret);
	ossControl.setBucketName(strBucket);
	ossControl.initOSS();

	tstring strToken = CController::Instance()->GetToken();
	
	tstring uploadName = infoFile.strFileFullName;          //
	//tstring strIdentifier = mainDlg::getInstance()->getCurIdentifier();
	//获取packid
	tstring strpack = infoFile.strPackID;
	tstring strPost;//("");
	tstring strResponse;
	CHttpClient HttpClient;
	//上传至服务器
	
	DWORD sz = infoFile.dwFileSize;
	//::EnterCriticalSection(&m_cs);
	tstring strOut;
	CChineseCode::GB2312ToUTF_8(strOut, infoFile.strFilePath.c_str(), infoFile.strFilePath.length());

	tstring strObject;
	CChineseCode::GB2312ToUTF_8(strObject, uploadName.c_str(), uploadName.length());
	//tstring strPath = infoFile.strFilePath;
	int res = ossControl.uploadPartFromFile(strObject, strOut, sz, infoFile.strKey, infoFile.stropposite);
	if (res == -1)
	{
		SetEvent(ossInterface::m_hThreadExit);

		//Close();
		return false;
	}
	
	if (res == OSS_SUCCESS)
	{
		
		//获取阿里云 url
		tstring struploadName = CChineseCode::EncodeUTF8(uploadName);
		tstring strUrl = ossControl.getFileUrl(struploadName);   //todo encode
		//strUrl = CChineseCode::EncodeUTF8(strUrl);
		infoFile.strFileUrl = strUrl;

		CString strSz;
		strSz.Format("%d", sz);
		
		
		infoFile.strFileSz  = strSz;
		HWND hMainWnd = mainDlg::getInstance()->GetHWND();
		fileInfo* pTmp = new fileInfo;
		pTmp->dwFileSize = infoFile.dwFileSize;
		pTmp->strFileFullName = infoFile.strFileFullName;
		pTmp->strFileName = infoFile.strFileName;
		pTmp->strFilePath = infoFile.strFilePath;
		pTmp->strFileSz = infoFile.strFileSz;
		pTmp->strFileType = infoFile.strFileType;
		pTmp->strFileUrl = infoFile.strFileUrl;
		pTmp->strKey = infoFile.strKey;
		pTmp->stropposite = infoFile.stropposite;
		pTmp->strPackID = infoFile.strPackID;
		pTmp->strPackName = infoFile.strPackName;
		::SendMessage(hMainWnd, WM_USER+199, (WPARAM)pTmp, 0);
	}
	else
	{
		return false;
	}
	return true;
}

bool CFileControl::downloadSignalFile( fileInfo& infoFile, IOssEventBase* pEventHandle)
{
	tstring strEndPoint("oss-cn-hangzhou.aliyuncs.com");
	tstring strAcessID("nHGOOiBnBsJCs04n");
	tstring strAcessIDSecret("Yod3tgIrcqVLpwuIk6yxRJKm56pvXr");
	tstring strBucket(" jp-mm");
	int res = 0;
	ossInterface ossControl;
	if (pEventHandle != NULL)
		ossControl.AddEventHandler(pEventHandle);
	ossControl.setAcessEndpoint(strEndPoint);
	ossControl.setAcessID(strAcessID);
	ossControl.setAcessIDSecret(strAcessIDSecret);
	ossControl.setBucketName(strBucket);
	ossControl.initOSS();


	res = ossControl.downloadObjectByUrl(infoFile.strFileUrl, CChineseCode::EncodeUTF8(infoFile.strFilePath), infoFile.dwFileSize, infoFile.strFileFullName, infoFile.strKey, infoFile.stropposite);
	if (res == OSS_SUCCESS)
	{
		mainDlg::getInstance()->addDownloadedFileMsg(infoFile);
		mainDlg::getInstance()->OnProcess(infoFile.strKey, 100, infoFile.stropposite);
		return true;
	}
	else
	{
		mainDlg::getInstance()->OnProcess(infoFile.strKey, -1, infoFile.stropposite);
		return false;
	}
}
