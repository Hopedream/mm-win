#pragma once

#include "StdAfx.h"
#include "UIDefine.h"
#include "oss_event.h"

#define WM_GROUPDOWNLOAD_PACKFILE_CLOSED		WM_USER+503

class CTileLayoutGroupPackList;
class CGroupPackFileDownloadWnd : public WindowImplBase
								, public IOssEventBase
{
public:
	CGroupPackFileDownloadWnd(CPaintManagerUI* ppm = NULL);
	virtual ~CGroupPackFileDownloadWnd();

	void SetMainWndHwnd(HWND hMainWnd)
	{
		m_hMainWnd = hMainWnd;
	}

	//注意：如果不加上const，就会报说该类仍然是虚类。
	virtual LPCTSTR GetWindowClassName()const	{	return _T("GroupMember_ListWnd");			}
	virtual CDuiString GetSkinFolder()			{	return _T("..\\MM\\res\\");					}
	virtual CDuiString GetSkinFile()			{	return "download_packfile_wnd.xml";			}
	virtual void OnFinalMessage( HWND hWnd )	{	delete this;								}
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CControlUI* CreateControl(LPCTSTR pstrClass);


	void CreateWnd();
	bool Initialize(sPackItem* pPackItem);

	virtual void OnProcess(tstring& fileName, int iValue, tstring &strKey);
	void SetRoomID(tstring& strRoomID);
protected:
	void OnDownLoadPackFiles();

	//文件下载相关
	bool downloadFiles(fileInfo& lstFiles, IOssEventBase* pEventHandle);
	static unsigned  __stdcall  downloadThreadFunc( void*pArguments);
	bool downloadSignalFile(fileInfo& infoFile, IOssEventBase* pEventHandle);

	bool IsDownLoading();
	bool IsExist(tstring& strFullFileName);
protected:
	CPaintManagerUI* m_pPaintManager;
	CTileLayoutGroupPackList* m_pGroupPackFileList;

	//当前packet的数据信息
	sPackItem* m_pCurrPackItem;
	bool m_bDownloaded;

	CRITICAL_SECTION m_csFState;
	enum EDownLoadState
	{	
		enm_DownState_OnGoing	= 1,
		enm_DownState_Finish	= 2,
		enm_DownState_Error		= 3,
	};
	map<tstring, EDownLoadState> m_mapFile2State;//本地的文件全称--下载状态
	HWND m_hMainWnd;
	tstring m_strRoomID; //哪个room的资料包
};