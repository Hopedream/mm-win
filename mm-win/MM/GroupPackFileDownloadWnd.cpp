#include "GroupPackFileDownloadWnd.h"
#include "TileLayoutGroupPackFileList.h"
#include "mainDlg.h"
#include "MUCRoomMgr.h"
#include "Controller.h"
#include "Utility.h"
#include <vector>
#include "HttpClient.h"
#include <ShellAPI.h>
#include "StringHelper.h"
#include "FileHelper.h"
#include "GroupChatPage.h"

struct sTmpInfo
{
	fileInfo* pFile;
	void* pThis;
	IOssEventBase* pEventHandle;
	tstring strProjectID;
	tstring strPackID;
	sTmpInfo()
	{
		pFile=NULL;
		pThis=NULL;
		pEventHandle=NULL;
	}
};


CGroupPackFileDownloadWnd::CGroupPackFileDownloadWnd(CPaintManagerUI* ppm/* = NULL*/)
{
	m_pPaintManager = ppm;
	m_pGroupPackFileList = NULL;
	m_pCurrPackItem = NULL;
	m_bDownloaded = false;
	::InitializeCriticalSection(&m_csFState);
}

CGroupPackFileDownloadWnd::~CGroupPackFileDownloadWnd()
{
	if (NULL != m_pCurrPackItem)
	{
		delete m_pCurrPackItem;
		m_pCurrPackItem = NULL;
	}
	::DeleteCriticalSection(&m_csFState);
}

void CGroupPackFileDownloadWnd::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		CDuiString strControlName = msg.pSender->GetName();

		//关闭界面
		if (_tcsicmp(msg.pSender->GetName(), "btn_close_downloadwnd") == 0)
		{
// 			if (NULL != m_hMainWnd)
// 			{
// 				::SendMessage(m_hMainWnd, WM_GROUPDOWNLOAD_PACKFILE_CLOSED, 0, 0);
// 			}
// 
// 			Close();
			ShowWindow(false);
		}
		//下载 所有文件
		else if (_tcsicmp(msg.pSender->GetName(), "btn_download_packfiles") == 0)
		{
			if (  IsDownLoading() || m_bDownloaded)
			{
				return;
			}
			
			sPackReadReceipt* pPackReadReceipt = new sPackReadReceipt();
			pPackReadReceipt->strPackID = m_pCurrPackItem->strPackID;
			pPackReadReceipt->strRoomName = m_strRoomID ;
			pPackReadReceipt->strFrom =  CController::Instance()->GetXmppClient()->jid().username();
			pPackReadReceipt->strPackName = m_pCurrPackItem->strPackName;
			pPackReadReceipt->strAttribute = m_pCurrPackItem->strPackProp;

			SYSTEMTIME sysTime;
			::GetLocalTime(&sysTime);
			char* pTime = new char[128];
			sprintf(pTime,"%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d",sysTime.wYear,sysTime.wMonth,sysTime.wDay,sysTime.wHour,sysTime.wMinute, sysTime.wSecond,sysTime.wMilliseconds,sysTime.wDayOfWeek);

			pPackReadReceipt->strTime = pTime;

			::SendMessage(mainDlg::getInstance()->GetHWND(), WM_GROUPDOWNLOAD_PACKFILE_DOWNLOADED, (WPARAM)pPackReadReceipt, 0);
 			
			OnDownLoadPackFiles();
		}
		//打开文件所在位置
		else if (_tcsicmp(msg.pSender->GetName(), "btn_open_downloaded_packfiles") == 0)
		{
			if (NULL != m_pCurrPackItem)
			{
				tstring strPackDir = CFileHelper::GetPackDir(m_pCurrPackItem->strPackName, m_pCurrPackItem->strPackID);
				if (!strPackDir.empty())
				{
					ShellExecute(NULL, NULL, strPackDir.c_str(), NULL,NULL,SW_SHOWNORMAL);
				}
			}
		}
		//打开，已下载文件
		else if (_tcsicmp(msg.pSender->GetName(), "btn_open_downloaded_file") == 0)
		{
			CContainerUI* pListItem  = static_cast<CContainerUI*>(msg.pSender->GetParent());
			if (NULL != pListItem)
			{
				char* pBuff = (char*)pListItem->GetTag();
				tstring strLocalFullFileName = pBuff;
				if (!strLocalFullFileName.empty())
				{
					ShellExecute(NULL, NULL, strLocalFullFileName.c_str(), NULL,NULL,SW_SHOWNORMAL);
				}
			}
		}
	}
	else if(_tcsicmp(msg.sType, _T("itemclick")) == 0)
	{
		//HandleItemClickEvent(msg);		
	}
	else if (_tcsicmp(msg.sType, _T("itemselect")) == 0)
	{
		
	}
	else if(_tcsicmp(msg.sType, _T("selectchanged")) == 0)
	{
		int bac=100;
	}
}

void CGroupPackFileDownloadWnd::SetRoomID(tstring& strRoomID)
{
	m_strRoomID = strRoomID;
}
//初始化窗口
void CGroupPackFileDownloadWnd::InitWindow()
{

}
LRESULT CGroupPackFileDownloadWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	bHandled = FALSE;
	return 0;
}

CControlUI* CGroupPackFileDownloadWnd::CreateControl(LPCTSTR pstrClass)
{
	if (_tcsicmp(pstrClass, "DownLoadPackFileList") == 0)
	{
		m_pGroupPackFileList = new CTileLayoutGroupPackList(m_pPaintManager);
		return m_pGroupPackFileList;
	}

	return NULL;
}

//根据PackID，获取文件列表，检查本地是否已经存在了。
bool CGroupPackFileDownloadWnd::Initialize(sPackItem* pPackItem)
{
	if (NULL == pPackItem)
	{
		return false;
	}

	//新建cache数据包的数据信息
	m_pCurrPackItem = new sPackItem;
	m_pCurrPackItem->listFile = pPackItem->listFile;
	m_pCurrPackItem->strPackID = pPackItem->strPackID;
	m_pCurrPackItem->strPackName = pPackItem->strPackName;
	m_pCurrPackItem->strPackProp = pPackItem->strPackProp;
	m_pCurrPackItem->strPackSender = pPackItem->strPackSender;
	m_pCurrPackItem->strPackSize = pPackItem->strPackSize;
	m_pCurrPackItem->strPackTime = pPackItem->strPackTime;

	//初始化标题栏
	CControlUI* pPackTitle = static_cast<CControlUI*>(m_pPaintManager->FindControl("label_gc_packname"));
	if (NULL != pPackTitle)
	{
		pPackTitle->SetText(pPackItem->strPackName.c_str());
	}
	
	//初始化文件列表
	if (pPackItem->listFile.size()>0)
	{
		for (list<sPackFileItem>::iterator it = pPackItem->listFile.begin();
			it != pPackItem->listFile.end(); ++it)
		{
			sPackFileItem& onePackFile = *it;
			m_pGroupPackFileList->AddOneItem(onePackFile);


		}
	}
	else
	{
		return false;
	}

	//检查文件是否已经下载了：如果下载了，就cache到m_mapFile2State，并更新UI
	if (pPackItem->listFile.size()>0)
	{
		tstring strPackDir = CFileHelper::GetPackDir(pPackItem->strPackName, pPackItem->strPackID);

		for (list<sPackFileItem>::iterator it = pPackItem->listFile.begin();
			it != pPackItem->listFile.end(); ++it)
		{
			sPackFileItem& oneFile = *it;
			tstring strFullPathName = strPackDir + it->strFileName;

			//先判断有没有这个文件了，如有就直接更新状态 "打开"可用
			bool bExist = PathFileExists(strFullPathName.c_str());
			if (bExist)
			{
				DWORD dwFileSize = _tcstoul(it->strFileSizeAccurate.c_str(), NULL, 10);

				if (getFileSize(strFullPathName) == dwFileSize)
				{
					if (!IsExist(strFullPathName))
					{
						::EnterCriticalSection(&m_csFState);
						m_mapFile2State.insert(make_pair(strFullPathName, enm_DownState_Finish));
						::LeaveCriticalSection(&m_csFState);

						//设置进度条为完成了。//说来话长，这个98就表示下载完成了...
						m_pGroupPackFileList->SetProgress(strFullPathName, 98);
					}
				}
			}
		}
	}
	
	


	return true;
}

void CGroupPackFileDownloadWnd::CreateWnd()
{
	Create(NULL, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);		
	CenterWindow();	
}

void CGroupPackFileDownloadWnd::OnDownLoadPackFiles()
{
	if (NULL != m_pGroupPackFileList && NULL != m_pCurrPackItem)
	{
		//if (!m_bDownloaded)
		//{
			int nFileCount = m_pCurrPackItem->listFile.size();

			for (list<sPackFileItem>::iterator it = m_pCurrPackItem->listFile.begin(); 
				it != m_pCurrPackItem->listFile.end(); ++it)
			{
				tstring strAliUrl = it->strFileUrl;
				tstring strFileName = it->strFileName;
				DWORD dwFileSize = _tcstoul(it->strFileSizeAccurate.c_str(), NULL, 10);

				fileInfo* pOneFile = new fileInfo;
				pOneFile->strFileUrl = strAliUrl;
				pOneFile->strFileName = strFileName;
				pOneFile->strFilePath = CFileHelper::GetPackDir(it->strPackName, it->strPackID);
				pOneFile->strFilePath += strFileName;//用户目录下的临时文件夹。
				pOneFile->dwFileSize = dwFileSize;

				//先判断有没有这个文件了，如有就直接更新状态为 ”打开“可用
				bool bExist = PathFileExists(pOneFile->strFilePath.c_str());
				if (bExist)
				{
					if (getFileSize(pOneFile->strFilePath) == dwFileSize)
					{
						if (!IsExist(pOneFile->strFilePath))
						{
							::EnterCriticalSection(&m_csFState);
							m_mapFile2State.insert(make_pair(pOneFile->strFilePath, enm_DownState_Finish));
							::LeaveCriticalSection(&m_csFState);

							//设置进度条为完成了。//说来话长，这个98就表示下载完成了...
							m_pGroupPackFileList->SetProgress(pOneFile->strFileFullName, 98);
						}
						
						delete pOneFile;
					}
				}
				else
				{
					//正在下载的文件项目
					::EnterCriticalSection(&m_csFState);
					m_mapFile2State.insert(make_pair(pOneFile->strFilePath, enm_DownState_OnGoing));
					::LeaveCriticalSection(&m_csFState);

					downloadFiles(*pOneFile, this);
				}
			}
		//}		
	}
}

bool CGroupPackFileDownloadWnd::downloadFiles( fileInfo& lstFiles, IOssEventBase* pEventHandle)
{
	HANDLE   hThread;  
	unsigned   threadID;   
	sTmpInfo* pTmp = new sTmpInfo;
	pTmp->pFile = &lstFiles;
	pTmp->pThis = (void*)this;
	pTmp->pEventHandle = pEventHandle;
	hThread = (HANDLE)_beginthreadex(NULL,0, downloadThreadFunc,pTmp,0,&threadID);
	CloseHandle(hThread); 
	return true;
}

unsigned  __stdcall CGroupPackFileDownloadWnd::downloadThreadFunc( void*pArguments )
{
	sTmpInfo* pTmp = (sTmpInfo*)pArguments;
	if (NULL != pTmp && NULL != pTmp->pThis)
	{
		((CGroupPackFileDownloadWnd*)(pTmp->pThis))->downloadSignalFile(*(pTmp->pFile), pTmp->pEventHandle);
	} 
	return   0;  
}

bool CGroupPackFileDownloadWnd::downloadSignalFile( fileInfo& infoFile, IOssEventBase* pEventHandle)
{
	tstring strEndPoint("oss-cn-hangzhou.aliyuncs.com");
	tstring strAcessID("nHGOOiBnBsJCs04n");
	tstring strAcessIDSecret("Yod3tgIrcqVLpwuIk6yxRJKm56pvXr");
	tstring strBucket(" jp-mm");
	int res = 0;
	ossInterface ossControl;
	ossControl.AddEventHandler(pEventHandle);
	ossControl.setAcessEndpoint(strEndPoint);
	ossControl.setAcessID(strAcessID);
	ossControl.setAcessIDSecret(strAcessIDSecret);
	ossControl.setBucketName(strBucket);
	ossControl.initOSS();


	res = ossControl.downloadObjectByUrl(infoFile.strFileUrl, 
		CChineseCode::EncodeUTF8(infoFile.strFilePath), 
		infoFile.dwFileSize, 
		infoFile.strFileFullName, 
		infoFile.strFileName/*infoFile.strKey*/, 
		tstring(_T(""))/*infoFile.stropposite*/);
	bool bSucc = res == OSS_SUCCESS;

	::EnterCriticalSection(&m_csFState);	
	map<tstring, EDownLoadState>::iterator it = m_mapFile2State.find(infoFile.strFilePath);
	if (it != m_mapFile2State.end())
	{
		if (bSucc)
		{
			it->second = enm_DownState_Finish;
		}
		else
		{
			it->second = enm_DownState_Error;
		}
	}
	::LeaveCriticalSection(&m_csFState);

	fileInfo* pFileInfo = &infoFile;
	if (NULL != pFileInfo)
	{
		delete pFileInfo;
	}

	return bSucc;
}

void CGroupPackFileDownloadWnd::OnProcess(tstring& fileName, int iValue, tstring &strKey)
{
	if (NULL != m_pGroupPackFileList && NULL != m_pCurrPackItem)
	{
		tstring strPackPath = CFileHelper::GetPackDir(m_pCurrPackItem->strPackName, m_pCurrPackItem->strPackID);
		//设置全路径进行比较
		m_pGroupPackFileList->SetProgress(strPackPath + fileName, iValue);
	}	
}

bool CGroupPackFileDownloadWnd::IsDownLoading()
{
	bool bOnGoing = false;
	::EnterCriticalSection(&m_csFState);
	for(map<tstring, EDownLoadState>::iterator it = m_mapFile2State.begin();
		it != m_mapFile2State.end(); ++it)
	{
		if (it->second == enm_DownState_OnGoing)
		{
			bOnGoing = true;
			break;
		}
	}
	::LeaveCriticalSection(&m_csFState);

	return bOnGoing;
}

bool CGroupPackFileDownloadWnd::IsExist(tstring& strFullFileName)
{
	bool bExist= false;
	::EnterCriticalSection(&m_csFState);
	
	map<tstring,EDownLoadState>::iterator itFind = m_mapFile2State.find(strFullFileName);
	bExist = itFind != m_mapFile2State.end();

	::LeaveCriticalSection(&m_csFState);

	return bExist;
}