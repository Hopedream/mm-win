#include "uWindow.h"
#include "common.h"
#include "json.h"
#include "Logger.h"
#include "StringHelper.h"
CDuiString uWindow::GetSkinFile()
{
	return _T("MMUpgrade.xml");
}

CDuiString uWindow::GetSkinFolder()
{
#ifdef _DEBUG
	return _T("..\\MM\\res\\");
#else
	//todo
	return _T("..\\MM\\res\\");
#endif
}

void uWindow::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void uWindow::OnClick( TNotifyUI& msg )
{
	WindowImplBase::OnClick(msg);
}

void uWindow::Notify( TNotifyUI& msg )
{
	WindowImplBase::Notify(msg);
}

void uWindow::CreateWnd(HWND hWnd)
{	
	//Create(hWnd, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);	
	Create(NULL, _T("MMUpgrade1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW , 0, 0, 600, 800);
	CenterWindow();
	ShowWindow(true);
}


LRESULT uWindow::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Close();
	bHandled = FALSE;
	return 0;
}
LRESULT uWindow::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return WindowImplBase::HandleMessage(uMsg, wParam, lParam);
}

LRESULT uWindow::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return WindowImplBase::HandleCustomMessage(uMsg, wParam, lParam, bHandled)/*0*/;
}

DuiLib::UILIB_RESOURCETYPE uWindow::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIP;
#endif
}


LPCTSTR uWindow::GetWindowClassName( void ) const
{	
	return _T("MMUpgrade");	
}


//
//struct para
//{
//	CProgressUI* pduiProgress;
//	CLabelUI* pduiLabel;
//	newversionfilelist* pNew;
//};

unsigned  __stdcall uWindow::ThreadFuncDownloadAllFiles(void* lpParamter)
{    
	uWindow* pThis = (uWindow*)(lpParamter);
	if (pThis->DoDownload())
	{
		
	}
    return 0;
}

//简易上传工具，后面最好做个更好的，用C#做都可以
void uWindow::DoUpload( string strVersion)
{
	string strEndPoint("oss-cn-hangzhou.aliyuncs.com");
	string strAcessID("nHGOOiBnBsJCs04n");
	string strAcessIDSecret("Yod3tgIrcqVLpwuIk6yxRJKm56pvXr");
	string strBucket("jp-mmupgrade-");
	strBucket = strBucket + strVersion;

	ossInterface ossControl;
	ossControl.InitGlobal();
	//ossControl.AddEventHandler((IOssEventBase*)this);
	ossControl.setAcessEndpoint(strEndPoint);
	ossControl.setAcessID(strAcessID);
	ossControl.setAcessIDSecret(strAcessIDSecret);
	ossControl.initOSS();
	ossControl.createBucket(strBucket,strEndPoint,OSS_ACL_PUBLIC_READ);
	ossControl.setBucketName(strBucket);
	//把sVersionFilePath里面的文件全部上传到阿里云
	vector<string> vecfilelst;
	CFileHelper::getSelectedFiles(vecfilelst);

	this->pNewVersion = new newversionfilelist();
	pNewVersion->strVersion = strVersion;

	for(vector<string>::iterator iter = vecfilelst.begin();iter!=vecfilelst.end();iter++)
	{
		string sCurFilePath = *iter;
		int iPos = sCurFilePath.find_last_of("\\");
		string sFileName = sCurFilePath.substr(iPos + 1);
		ossControl.uploadObjectFromFile(sFileName,sCurFilePath);

		fileinfo* pPI = new fileinfo();
		pPI->dwFileSize = CFileHelper::getFileSize(sCurFilePath);
		pPI->strFileName = sFileName;
		pPI->strFolder = "."; //这个地方需要斟酌，怎么弄比较好
		
		string struploadName = CChineseCode::EncodeUTF8(sFileName);
		string strUrl = ossControl.getFileUrl(struploadName);
		pPI->strUrl = strUrl;

		pNewVersion->vecFiles.push_back(*pPI);
	}

	//匹配文件夹pPI->strFolder
	string strPathIniFile;
	CFileHelper::getPathIniFile(strPathIniFile);
	for(vector<fileinfo>::iterator iter = pNewVersion->vecFiles.begin();iter!= pNewVersion->vecFiles.end();iter++)
	{
		string strFileName = iter->strFileName;
		string strDesFolder;
		TCHAR szDesFolder[MAX_PATH];
		GetPrivateProfileString("PATH",strFileName.c_str(),".",szDesFolder,MAX_PATH,strPathIniFile.c_str());
		strDesFolder = szDesFolder;
		iter->strFolder = strDesFolder;
	}

	//上传完后把文件列表及url传给web后台
	common* pcommon = new common();
	string sNextVersionInfo;
	sNextVersionInfo = "version={\"version\": \"";
	sNextVersionInfo = sNextVersionInfo + pNewVersion->strVersion;
	sNextVersionInfo = sNextVersionInfo + "\"";
	sNextVersionInfo = sNextVersionInfo + ",\"filelist\": [";

	for(vector<fileinfo>::iterator iter = pNewVersion->vecFiles.begin();iter!= pNewVersion->vecFiles.end();iter++)
	{
		sNextVersionInfo = sNextVersionInfo +"{\"filename\": \"";
		sNextVersionInfo = sNextVersionInfo + iter->strFileName;
		sNextVersionInfo = sNextVersionInfo + "\",\"path\":\"";
		sNextVersionInfo = sNextVersionInfo + iter->strFolder;
		sNextVersionInfo = sNextVersionInfo + "\",\"filesize\":";

		CDuiString sSize;
		sSize.Format("%d",iter->dwFileSize);
		string strFileSize = sSize;
		sNextVersionInfo = sNextVersionInfo + strFileSize;
		sNextVersionInfo = sNextVersionInfo + ",\"url\": \"";
		sNextVersionInfo = sNextVersionInfo + iter->strUrl;
		sNextVersionInfo = sNextVersionInfo + "\"},";
	}
	sNextVersionInfo = sNextVersionInfo.substr(0,sNextVersionInfo.size() - 1);
	sNextVersionInfo = sNextVersionInfo + "]}";

	//string strUrlEncode = CChineseCode::UrlEncode(sNextVersionInfo);
	pcommon->SetVerison(sNextVersionInfo);
	pcommon->~common();
	delete pcommon;

	ossControl.deinitializeOSS();
	this->Close();
}

void uWindow::DoUpdate( void )
{	
	//sample
	//{"version": "102","filelist": [{"filename": "1.dll","path":".","filesize":0,"url": "http://jp-mmupgrade-102.oss-cn-hangzhou.aliyuncs.com:80/1.dll?OSSAccessKeyId=nHGOOiBnBsJCs04n&Expires=1783146093&Signature=oCZnSMoyzWDdCAgkTAvSHpzAD/w%3D"},{"filename": "2.dll","path":".","filesize":0,"url": "http://jp-mmupgrade-102.oss-cn-hangzhou.aliyuncs.com:80/2.dll?OSSAccessKeyId=nHGOOiBnBsJCs04n&Expires=1783146093&Signature=O%2B/l2SPGcP6LBWyXmbcSKmW9fAs%3D"},{"filename": "3.dll","path":".","filesize":0,"url": "http://jp-mmupgrade-102.oss-cn-hangzhou.aliyuncs.com:80/3.dll?OSSAccessKeyId=nHGOOiBnBsJCs04n&Expires=1783146093&Signature=FSSL/FSBMxEKQejYdD/lmbYcBNM%3D"},{"filename": "x.exe","path":".","filesize":0,"url": "http://jp-mmupgrade-102.oss-cn-hangzhou.aliyuncs.com:80/x.exe?OSSAccessKeyId=nHGOOiBnBsJCs04n&Expires=1783146093&Signature=vE0LdDIp1DnPZ4KH3idEXjhxN8Q%3D"}]}
	common* pcommon = new common();
	string sJson = pcommon->GetVersion();
	sJson = pcommon->replace_all_distinct(sJson,"\\\"","\"");
	//sJson = sJson.substr(1,sJson.length() -1); //为什么会多俩引号?后台?
	pcommon->~common();
	delete pcommon;

	//sJson= "{\"version\": \"103\",\"filelist\": [{\"filename\": \"CameraDll.dll\",\"path\":\".\\\\config\",\"filesize\":135168,\"url\": \"http://jp-mmupgrade-103.oss-cn-hangzhou.aliyuncs.com:80/CameraDll.dll?OSSAccessKeyId=nHGOOiBnBsJCs04n&Expires=1783152247&Signature=EVNzuufEopSTT17JTkt9gFpsPlo%3D\"},{\"filename\": \"localdb.sqlite\",\"path\":\".\\\\sound\",\"filesize\":514048,\"url\": \"http://jp-mmupgrade-103.oss-cn-hangzhou.aliyuncs.com:80/localdb.sqlite?OSSAccessKeyId=nHGOOiBnBsJCs04n&Expires=1783152248&Signature=3ZsrUIXOzkO074HDL3/PahRGYgQ%3D\"}]}";

	this->pNewVersion = new newversionfilelist();
	CLabelUI* pLUI = static_cast<CLabelUI*>(m_PaintManager.FindControl("LabelU"));
	string strLabel = "获取版本信息发生错误...";

	Json::Reader reader;
	Json::Value root;
	try
	{
		if (reader.parse(sJson, root))
		{
			pNewVersion->strVersion = root["version"].asString();
			int nFileCount = root["filelist"].size();

			if (pNewVersion->strVersion == ""||nFileCount ==0)
			{
				pLUI->SetText(strLabel.c_str());
				return;
			}

			for(int i=0;i<nFileCount;i++)
			{
				fileinfo* pFI = new fileinfo();
				pFI->strFileName = root["filelist"][i]["filename"].asString();
				pFI->strFolder = root["filelist"][i]["path"].asString();
				pFI->dwFileSize = root["filelist"][i]["filesize"].asInt();
				pFI->strUrl = root["filelist"][i]["url"].asString();
				pNewVersion->vecFiles.push_back(*pFI);
			}
		}
	}
	catch (...)
	{
		//please do something
		CLogger::GetInstance()->PrintErrLog( "MMUpdate -->parse GetVersion Response exception: %s", sJson.c_str());		
		pLUI->SetText(strLabel.c_str());
		return;
	}
	downloadFiles();
}
void uWindow::downloadFiles()
{
	HANDLE   hThread;  
	unsigned   threadID;   
	hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFuncDownloadAllFiles, this, 0, &threadID);
	CloseHandle(hThread);
}

int uWindow::DoDownload()
{
	int nResult = 0;

	string strEndPoint("oss-cn-hangzhou.aliyuncs.com");
	string strAcessID("nHGOOiBnBsJCs04n");
	string strAcessIDSecret("Yod3tgIrcqVLpwuIk6yxRJKm56pvXr");
	string strBucket("jp-mmupgrade-");
	strBucket = strBucket + pNewVersion->strVersion;

	ossInterface ossControl;
	ossControl.InitGlobal();
	ossControl.AddEventHandler((IOssEventBase*)this);
	ossControl.setAcessEndpoint(strEndPoint);
	ossControl.setAcessID(strAcessID);
	ossControl.setAcessIDSecret(strAcessIDSecret);
	ossControl.setBucketName(strBucket);
	ossControl.initOSS();

	string sAppPath = CFileHelper::GetCurrentAppPath();
	string sUpdateVersionPath = sAppPath + "update\\" + pNewVersion->strVersion;
	CFileHelper::CreateFolder(sUpdateVersionPath.c_str());
	string sEmpty = "";
	for(vector<fileinfo>::iterator iter = pNewVersion->vecFiles.begin();iter!= pNewVersion->vecFiles.end();iter++)
	{
		nResult = ossControl.downloadObjectByUrl(iter->strUrl,sUpdateVersionPath+"\\"+iter->strFileName, iter->dwFileSize, iter->strFileName, sEmpty,sEmpty);
	}
	ossControl.deinitializeOSS();
	if(nResult == 200)
	{
		CopyFiles();
	}
	return nResult;
}

void uWindow::OnProcess(string& fileName, int iValue , string &strKey)
{
	CLabelUI* pLUI = static_cast<CLabelUI*>(m_PaintManager.FindControl("LabelU"));
	CProgressUI* pPUI = static_cast<CProgressUI*>(m_PaintManager.FindControl("ProgressU"));
	
	string strLabel = "正在下载" + fileName + "...";
	pLUI->SetText(strLabel.c_str());
	pPUI->SetValue(iValue);
}

void uWindow::CopyFiles()
{
	string sAppPath = CFileHelper::GetCurrentAppPath();
	for(vector<fileinfo>::iterator iter = pNewVersion->vecFiles.begin();iter!= pNewVersion->vecFiles.end();iter++)
	{
		string sFilePath = sAppPath + "update\\" + pNewVersion->strVersion+"\\" + iter->strFileName;
		string sDestPath = sAppPath + "\\" + iter->strFolder + "\\"+ iter->strFileName;
		::CopyFile(sFilePath.c_str(),sDestPath.c_str(),false);
	}
	CLabelUI* pLUI = static_cast<CLabelUI*>(m_PaintManager.FindControl("LabelU"));
	pLUI->SetText("更新完毕，即将启动MM！");
	::Sleep(1000);
	//写到ini，避免循环更新
	string sIniFile = sAppPath + "System.ini";
	WritePrivateProfileString("version","version",pNewVersion->strVersion.c_str(),sIniFile.c_str());

	string sMM = sAppPath + "MM.exe";
	::ShellExecute(NULL, NULL, sMM.c_str(), pNewVersion->strVersion.c_str(), NULL, SW_SHOWNORMAL);
	::Sleep(1000);
	this->Close();
}


