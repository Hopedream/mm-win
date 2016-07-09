#include "UIUpdateFiles.h"
#include <ShellAPI.h>
#include "mainDlg.h"
#include "Controller.h"
#include <sstream>
#include "../Common/Logger.h"
#include "mainDlg.h"
#include <atlstr.h>

UIUPdateFiles::UIUPdateFiles( LPCTSTR pszXMLPath ):
		m_strXMLPath(pszXMLPath),
		m_iTaskNum(0),
		m_iCurTaskNum(0),
		m_bAdd(false),
		m_strPackID(""),
		m_bStarted(false)
{
	m_pTileLayout=NULL; 
	initOSS();
}

UIUPdateFiles::~UIUPdateFiles()
{
	delete m_pTileLayout; 
	m_pTileLayout=NULL;
}

void UIUPdateFiles::Notify( TNotifyUI& msg )
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), _T("buttonClose")) == 0)
		{
			if (m_bStarted)
			{
				ossInterface::StopTask();
				if(WaitForSingleObject(ossInterface::m_hThreadExit,INFINITE) == WAIT_OBJECT_0)
				{

				}
			}
			ShowWindow(false);
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("buttonCancel")) == 0)
		{
			if (m_bStarted)
			{
				ossInterface::StopTask();
				if(WaitForSingleObject(ossInterface::m_hThreadExit,INFINITE) == WAIT_OBJECT_0)
				{

				}
			}
			ShowWindow(false);
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("buttonSend")) == 0)
		{
			CButtonUI *pButtonSend = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("buttonSend")));
			pButtonSend->SetEnabled(false);
			CButtonUI *pButtonAdd = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("buttonAdd")));
			pButtonAdd->SetEnabled(false);
			m_bStarted = true;
			uploadFiles();
		}
		else if (_tcsicmp(msg.pSender->GetName(), _T("buttonAdd")) == 0)
		{
			m_bAdd = true;
			vector<tstring> filelst;
			getSelectedFiles(filelst);
			if (filelst.size() > 0)
				addItem(filelst);
		}
	}
}


void UIUPdateFiles::addItem( vector<tstring>& lstfiles)
{
	CButtonUI *pButtonOpen = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("buttonOpenFile")));
	pButtonOpen->SetVisible(false);
	int iCount = lstfiles.size();
	if (iCount <= 0)
		return;
	else
	{
		if (!m_bAdd)
		{
			tstring strToken = CController::Instance()->GetToken();
			//tstring uploadName = infoFile.strFileFullName;          //
			tstring strIdentifier = mainDlg::getInstance()->getCurIdentifier();
			//获取packid
			tstring strUrl(WEB_SERVER_BASE_URL);
			strUrl += "file/create-file-pack?identifier=";
			strUrl += strIdentifier;
			//tstring strPost;//("");
			tstring strResponse;
			CHttpClient HttpClient;
			HttpClient.Get(strUrl, strToken, strResponse);

			//截取packid
			tstring strpack;
			strpack = strResponse.substr(strResponse.find("\"id\":\"") + 6, strResponse.length());
			m_strPackID = strpack.substr(0, strpack.find("\""));
			m_bAdd = false;
		}
		//获取packid

		CString cstrLog;
		cstrLog.Format("----------getpackid %s", m_strPackID.c_str());
		CLogger::GetInstance()->PrintLog(cstrLog, ENMLOGTYPE::LOG_INFO);

		for (int i=0; i<iCount; i++)
		{
			CDialogBuilder m_DlgBuilder;
			CVerticalLayoutUI *plstItem = NULL;
			tstring filePath = lstfiles[i];//->strFilePath;
			//todo chinese
			//CChineseCode::UTF_8ToGB2312(strOut, (char*)oneChatMsg.strChatText.c_str(), oneChatMsg.strChatText.size());
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
			plstItem->SetName(filePath.c_str());

			if (m_pTileLayout == NULL)
				m_pTileLayout = static_cast<CTileLayoutUI*>(m_PaintManager.FindControl(_T("TileLayoutUI1")));
			if (!m_pTileLayout->Add(plstItem))
			{
				delete plstItem;
			}

			//setItemBkImage();   todo
			tstring strValue = filePath.c_str();
			//截取文件名
			//1. 去除文件夹
			int iPos=strValue.find_last_of("\\");
			//i += 2;
			strValue = strValue.substr(iPos + 1); //strValue.Mid(i, strValue.GetLength());
			CDuiString strFileFullName = strValue.c_str();
			//2. 去除文件类型后缀
			iPos = strValue.find_last_of('.');
			CDuiString strfileType = strValue.substr(iPos+1).c_str();
			CDuiString strFileName = strValue.substr(0, iPos).c_str();
			//CDuiString strFileName = strValue.Left(i);

			setItemText(filePath, tstring(strFileName));


			fileInfo infoValue;
			infoValue.strFileFullName = strFileFullName;
			infoValue.strFileName = strFileName;
			infoValue.strFilePath = filePath;
			infoValue.strFileType = strfileType;
			infoValue.strPackID = m_strPackID;
			m_lstFiles.push_back(infoValue);
			//获取文件缩略图
			//SHFILEINFO sfi;
			//SHGetFileInfo(filePath.c_str(), 0, &sfi, sizeof(sfi), SHGFI_ICON | SHGFI_LARGEICON);
			////nImage = m_imglist.AddIcon(sfi.hIcon);
			//HICON icon = sfi.hIcon;
			//DestroyIcon(sfi.hIcon);
		}
	}	
}

int UIUPdateFiles::getItemIndex( tstring& strName)
{
	CVerticalLayoutUI *plstItem = NULL;
	plstItem = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindSubControlByName(m_pTileLayout, strName.c_str()));
	if (plstItem == NULL)
		return -1;
	int iIndex = m_pTileLayout->GetItemIndex(plstItem);
	return iIndex;
}

void UIUPdateFiles::setItemProcess( tstring &strPath,int iValue )
{
	CVerticalLayoutUI *plstItem = NULL;
	plstItem = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindSubControlByName(m_pTileLayout, strPath.c_str()));
	CProgressUI *pProcess = static_cast<CProgressUI*>(m_PaintManager.FindSubControlByName(plstItem, _T("progressUpload")));
	pProcess->SetValue(iValue);
	if (iValue == 100)
	{
		m_iCurTaskNum++;
		if (m_iCurTaskNum == m_iTaskNum)   //全部上传完成
		{
			//MessageBox(NULL, _T("上传完成！"), _T("提示"), MB_OK);
			//mainDlg::getInstance()->addFileMsg(m_lstFiles);   
			ShowWindow(false);
		}
	}
}

void UIUPdateFiles::setItemBkImage( tstring &strPath, tstring & strImage )
{
	//ContainerUI1
	CVerticalLayoutUI *plstItem = NULL;
	plstItem = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindSubControlByName(m_pTileLayout, strPath.c_str()));
	CContainerUI *pProcess = static_cast<CContainerUI*>(m_PaintManager.FindSubControlByName(plstItem, _T("ContainerUI1")));
	pProcess->SetBkImage(strImage.c_str());

}

void UIUPdateFiles::setItemText( tstring &strPath, tstring& strTxt )
{
	//labelFile
	CVerticalLayoutUI *plstItem = NULL;
	plstItem = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindSubControlByName(m_pTileLayout, strPath.c_str()));
	CLabelUI *pProcess = static_cast<CLabelUI*>(m_PaintManager.FindSubControlByName(plstItem, _T("labelFile")));
	pProcess->SetText(strTxt.c_str());
}

int UIUPdateFiles::getItemProcess( tstring &strPath )
{
	CVerticalLayoutUI *plstItem = NULL;
	plstItem = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindSubControlByName(m_pTileLayout, strPath.c_str()));
	CProgressUI *pProcess = static_cast<CProgressUI*>(m_PaintManager.FindSubControlByName(plstItem, _T("progressUpload")));
	return pProcess->GetValue();
}

int UIUPdateFiles::initOSS()
{
	//m_ossControl.AddEventHandler((IOssEventBase*)this);

	//InitializeCriticalSection(&m_cs);
	return 1;
}

void UIUPdateFiles::OnProcess(tstring& fileName, int iValue )
{
	setItemProcess(fileName, iValue);
}
struct sTmpInfo
{
	fileInfo* pFile;
	void* pThis;
	sTmpInfo()
	{
		pFile=NULL;
		pThis=NULL;
	}
};
void UIUPdateFiles::uploadFiles()
{
	m_iTaskNum = m_lstFiles.size();
	for (int i=0; i<m_iTaskNum; i++)
	{
		HANDLE   hThread;  
		unsigned   threadID;   

		sTmpInfo* pTmp = new sTmpInfo;
		pTmp->pFile = &m_lstFiles[i];
		pTmp->pThis = (void*)this;
		hThread = (HANDLE)_beginthreadex(NULL,0,SecondThreadFunc,pTmp,0,&threadID);
		CloseHandle(hThread); 
	}
}

unsigned  __stdcall UIUPdateFiles::SecondThreadFunc(   void*   pArguments   )  
{  
	sTmpInfo* pTmp = (sTmpInfo*)pArguments;
	if (NULL != pTmp && NULL != pTmp->pThis)
	{
		((UIUPdateFiles*)(pTmp->pThis))->uploadSignalFile(*(pTmp->pFile));
	}
	//_endthreadex(0);  
	return   0;  
}  

void UIUPdateFiles::uploadSignalFile(fileInfo& infoFile)
{
	//Init oss.
	tstring strEndPoint("oss-cn-hangzhou.aliyuncs.com");
	tstring strAcessID("nHGOOiBnBsJCs04n");
	tstring strAcessIDSecret("Yod3tgIrcqVLpwuIk6yxRJKm56pvXr");
	tstring strBucket("jp-mm");

	ossInterface ossControl;
	ossControl.AddEventHandler((IOssEventBase*)this);
	ossControl.setAcessEndpoint(strEndPoint);
	ossControl.setAcessID(strAcessID);
	ossControl.setAcessIDSecret(strAcessIDSecret);
	ossControl.setBucketName(strBucket);
	ossControl.initOSS();

	tstring strToken = CController::Instance()->GetToken();
	
	tstring uploadName = infoFile.strFileFullName;          //
	tstring strIdentifier = mainDlg::getInstance()->getCurIdentifier();
	//获取packid
	tstring strpack = infoFile.strPackID;
	tstring strPost;//("");
	tstring strResponse;
	CHttpClient HttpClient;
	//上传至服务器
	
	DWORD sz = infoFile.dwFileSize;
	//::EnterCriticalSection(&m_cs);
	int res = ossControl.uploadPartFromFile(uploadName, infoFile.strFilePath, sz, infoFile.strKey, infoFile.stropposite);
	if (res == -1)
	{
		SetEvent(ossInterface::m_hThreadExit);

		//Close();
		return;
	}
	
	if (res == OSS_SUCCESS)
	{
		
		//获取阿里云 url
		tstring strUrl = ossControl.getFileUrl(uploadName);   //todo encode
		strUrl = UrlEncode(strUrl);
	/*	for (int i=0; i<strUrl.size(); i++)
		{
			if (strUrl[i] == '&')
			{
				strUrl.replace(i, 1, "'*'");
				i++;
			}
		}*/

		//保存文件信息
		tstring strFileFullName = infoFile.strFileFullName;
		tstring strParam(WEB_SERVER_BASE_URL);
		strParam += "file/save-pack-files";

		CString strSz;
		strSz.Format("%d", sz);
		
		tstring strPost = "pack_id=" + strpack + "&file_datas="
			+"[{" + '"'+"file_name" +'"'+":" + '"'+strFileFullName+'"' +','+"\"file_size\":\""+ strSz.GetBuffer(0) +"\"" +',' + '"'+ "file_type"+'"' +":" + '"'+infoFile.strFileType+'"'+',' +'"'+ "ali_file_url"+'"'+ ":" + '"'+strUrl+'"' + "}]";
		//tstring strPost;
		//strPost = UrlEncode(strPost); 
		HttpClient.Post(strParam,strPost, strToken, strResponse );	
		// 操作全部完成 进度设置为100%
		//MessageBox(NULL, _T("100%！"), _T("提示"), MB_OK);
		OnProcess(infoFile.strFilePath, 100);
	}
	//::LeaveCriticalSection(&m_cs);
}

std::tstring UIUPdateFiles::UrlEncode( const std::tstring& szToEncode )
{
	std::tstring src = szToEncode;  
	char hex[] = "0123456789ABCDEF";  
	tstring dst;  


	for (size_t i = 0; i < src.size(); ++i)  
	{  
		unsigned char cc = src[i];  
		if ( cc >= 'A' && cc <= 'Z'   
			|| cc >='a' && cc <= 'z'  
			|| cc >='0' && cc <= '9'  
			|| cc == '.'  
			|| cc == '_'  
			|| cc == '-'  
			|| cc == '*')  
		{  
			if (cc == ' ')  
			{  
				dst += "+";  
			}  
			else  
				dst += cc;  
		}  
		else  
		{  
			unsigned char c = static_cast<unsigned char>(src[i]);  
			dst += '%';  
			dst += hex[c / 16];  
			dst += hex[c % 16];  
		}  
	}  
	return dst;  
}

void UIUPdateFiles::RemoveAll()
{
	m_lstFiles.clear();
	if (m_pTileLayout != NULL)
		m_pTileLayout->RemoveAll();

	CButtonUI *pButtonSend = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("buttonSend")));
	if (pButtonSend != NULL)
		pButtonSend->SetEnabled(true);
	CButtonUI *pButtonAdd = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("buttonAdd")));
	if (pButtonAdd != NULL)
		pButtonAdd->SetEnabled(true);
	m_bStarted = false;
	m_iCurTaskNum = 0;
}

