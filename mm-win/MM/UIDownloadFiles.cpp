#include "UIDownloadFiles.h"
//#include "UIUpdateFiles.h"
//#include <ShellAPI.h>
#include "mainDlg.h"
#include "FileHelper.h"
#include <ShellAPI.h>
//#include "Controller.h"
//#include <sstream>
//
//
UIDownloadFiles::UIDownloadFiles( LPCTSTR pszXMLPath ):
		m_strXMLPath(pszXMLPath),
		m_iTaskNum(0),
		m_iCurTaskNum(0),
		m_strFilePath(_T(""))
{
	m_pTileLayout=NULL; 
	//initOSS();
}
//
void UIDownloadFiles::Notify( TNotifyUI& msg )
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), _T("buttonClose")) == 0)
		{
			ShowWindow(false);
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("buttonCancel")) == 0)
		{
			ShowWindow(false);
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("buttonOpenFile")) == 0)
		{
			//打开文件夹
			if (m_strFilePath != "")
				ShellExecute(NULL, "explore", m_strFilePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
	}
}
//
//
void UIDownloadFiles::addItem( vector<fileInfo>& lstfiles)
{
	CButtonUI *pButtonSend = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("buttonSend")));
	pButtonSend->SetVisible(false);
	CButtonUI *pButtonAdd = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("buttonAdd")));
	pButtonAdd->SetVisible(false);


	m_lstDownloadFiles = lstfiles;
	int iCount = lstfiles.size();
	if (iCount <= 0)
		return;
	else
	{
		for (int i=0; i<iCount; i++)
		{
			CDialogBuilder m_DlgBuilder;
			CVerticalLayoutUI *plstItem = NULL;
			tstring fileFullName = lstfiles[i].strFileFullName;//->strFilePath;
			if (!m_DlgBuilder.GetMarkup()->IsValid())
			{
				plstItem = static_cast<CVerticalLayoutUI*>(m_DlgBuilder.Create(_T("updateProcessItem.xml"),
					(UINT)0, NULL, &m_PaintManager));
			}
			else
			{
				plstItem = static_cast<CVerticalLayoutUI*>(m_DlgBuilder.Create((UINT)0, &m_PaintManager));
			}
			if (plstItem == NULL)
				return;
			plstItem->SetName(fileFullName.c_str());

			if (m_pTileLayout == NULL)
				m_pTileLayout = static_cast<CTileLayoutUI*>(m_PaintManager.FindControl(_T("TileLayoutUI1")));
			if (!m_pTileLayout->Add(plstItem))
			{
				delete plstItem;
			}

			//setItemBkImage();   todo
			
			//i += 2;
			//strValue = strValue.substr(iPos + 1); //strValue.Mid(i, strValue.GetLength());
			//CDuiString strFileFullName = strValue.c_str();
			//2. 去除文件类型后缀
			int iPos = fileFullName.find_last_of('.');
			CDuiString strfileType = fileFullName.substr(iPos+1).c_str();
			CDuiString strFileName = fileFullName.substr(0, iPos).c_str();
			//CDuiString strFileName = strValue.Left(i);

			setItemText(fileFullName, tstring(strFileName));	
			//获取文件缩略图
			//SHFILEINFO sfi;
			//SHGetFileInfo(filePath.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON);
			////nImage = m_imglist.AddIcon(sfi.hIcon);
			//HICON icon = sfi.hIcon;
			//DestroyIcon(sfi.hIcon);
		}
	}	
}

void UIDownloadFiles::downloadFiles()
{
	////开始下载文件  起下载线程
	HANDLE   hThread;  
	unsigned   threadID;   
	hThread = (HANDLE)_beginthreadex(NULL, 0, DownLoadThreadFunc, this, 0, &threadID);
	CloseHandle(hThread); 
}

//
int UIDownloadFiles::getItemIndex( tstring& strName)
{
	CVerticalLayoutUI *plstItem = NULL;
	plstItem = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindSubControlByName(m_pTileLayout, strName.c_str()));
	if (plstItem == NULL)
		return -1;
	int iIndex = m_pTileLayout->GetItemIndex(plstItem);
	return iIndex;
}
//
void UIDownloadFiles::setItemProcess( tstring &strPath,int iValue )
{
	CVerticalLayoutUI *plstItem = NULL;
	plstItem = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindSubControlByName(m_pTileLayout, strPath.c_str()));
	CProgressUI *pProcess = static_cast<CProgressUI*>(m_PaintManager.FindSubControlByName(plstItem, _T("progressUpload")));
	pProcess->SetValue(iValue);
	if (iValue == 100)
	{
		hideItemProcess(strPath);
		m_iCurTaskNum++;
		if (m_iCurTaskNum == m_iTaskNum)   //全部上传完成
		{
			MessageBox(NULL, _T("下载完成！"), _T("提示"), MB_OK);
			//mainDlg::getInstance()->addFileMsg(m_lstFiles);   
			//Close();
		}

	}
}
//
void UIDownloadFiles::setItemBkImage( tstring &strPath, tstring & strImage )
{
	//ContainerUI1
	CVerticalLayoutUI *plstItem = NULL;
	plstItem = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindSubControlByName(m_pTileLayout, strPath.c_str()));
	CContainerUI *pProcess = static_cast<CContainerUI*>(m_PaintManager.FindSubControlByName(plstItem, _T("ContainerUI1")));
	pProcess->SetBkImage(strImage.c_str());

}
//
void UIDownloadFiles::setItemText( tstring &strPath, tstring& strTxt )
{
	//labelFile
	CVerticalLayoutUI *plstItem = NULL;
	plstItem = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindSubControlByName(m_pTileLayout, strPath.c_str()));
	CLabelUI *pProcess = static_cast<CLabelUI*>(m_PaintManager.FindSubControlByName(plstItem, _T("labelFile")));
	pProcess->SetText(strTxt.c_str());
}
//
int UIDownloadFiles::getItemProcess( tstring &strPath )
{
	CVerticalLayoutUI *plstItem = NULL;
	plstItem = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindSubControlByName(m_pTileLayout, strPath.c_str()));
	CProgressUI *pProcess = static_cast<CProgressUI*>(m_PaintManager.FindSubControlByName(plstItem, _T("progressUpload")));
	return pProcess->GetValue();
}
//
//int UIUPdateFiles::initOSS()
//{
//	//m_ossControl.AddEventHandler((IOssEventBase*)this);
//
//	//InitializeCriticalSection(&m_cs);
//	return 1;
//}
//
void UIDownloadFiles::OnProcess(tstring& fileName, int iValue )
{
	setItemProcess(fileName, iValue);
}

unsigned  __stdcall UIDownloadFiles::DownLoadThreadFunc( void*pArguments )
{
	UIDownloadFiles* pThis = (UIDownloadFiles*)(pArguments);
	if (pThis->downloadFile())
	{
		//MessageBox(NULL, _T("下载完成！"), _T("提示"), MB_OK);
	}

	return 0;
}

int UIDownloadFiles::downloadFile()
{
	tstring strEndPoint("oss-cn-hangzhou.aliyuncs.com");
	tstring strAcessID("nHGOOiBnBsJCs04n");
	tstring strAcessIDSecret("Yod3tgIrcqVLpwuIk6yxRJKm56pvXr");
	tstring strBucket(" jp-mm");
	int res = 0;
	//m_mapDownList.clear();
	for (int i=0; i<m_lstDownloadFiles.size(); i++)
	{
		//::SetTimer( NULL, i, 200, (TIMERPROC)TimerProc );
		//m_mapDownList.insert(make_pair(i, m_lstDownloadFiles[i]));
		tstring strUrl = m_lstDownloadFiles[i].strFileUrl;
		tstring strFileName = m_lstDownloadFiles[i].strFileFullName;

		tstring strBasePath = CFileHelper::GetUserDir() +"User\\";  //+ m_MyselfItem.m_strAccount + "\\RecFiles\\";
		if (!fopen(strBasePath.c_str(), "w") )
		{
			CreateDirectory(strBasePath.c_str(), NULL);
		}
		strBasePath += mainDlg::getInstance()->getMyselfItem().m_strAccount + "\\";
		if (!fopen(strBasePath.c_str(), "w"))
		{
			CreateDirectory(strBasePath.c_str(), NULL);
		}
		strBasePath += "RecvFiles\\";
		if (!fopen(strBasePath.c_str(), "w"))
		{
			CreateDirectory(strBasePath.c_str(), NULL);
		}
		m_strFilePath = strBasePath;
		strBasePath += strFileName;
		//"D:\\Projects\\mm-win\\bin\\131\\" + strFileName;
		ossInterface ossControl;
		ossControl.AddEventHandler((IOssEventBase*)this);
		ossControl.setAcessEndpoint(strEndPoint);
		ossControl.setAcessID(strAcessID);
		ossControl.setAcessIDSecret(strAcessIDSecret);
		ossControl.setBucketName(strBucket);
		ossControl.initOSS();
		res = ossControl.downloadObjectByUrl(strUrl, strBasePath, m_lstDownloadFiles[i].dwFileSize, strFileName, m_lstDownloadFiles[i].strKey, m_lstDownloadFiles[i].stropposite);
	}
	return res;
}

void UIDownloadFiles::hideItemProcess( tstring &strPath )
{
	CVerticalLayoutUI *plstItem = NULL;
	plstItem = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindSubControlByName(m_pTileLayout, strPath.c_str()));
	CProgressUI *pProcess = static_cast<CProgressUI*>(m_PaintManager.FindSubControlByName(plstItem, _T("progressUpload")));
	pProcess->SetVisible(false);
}

void UIDownloadFiles::RemoveAll()
{
	if (m_pTileLayout != NULL)
		m_pTileLayout->RemoveAll();
	m_iCurTaskNum = 0;
	m_lstDownloadFiles.clear();
}
