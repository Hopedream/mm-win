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

	//ע�⣺���������const���ͻᱨ˵������Ȼ�����ࡣ
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

	//�ļ��������
	bool downloadFiles(fileInfo& lstFiles, IOssEventBase* pEventHandle);
	static unsigned  __stdcall  downloadThreadFunc( void*pArguments);
	bool downloadSignalFile(fileInfo& infoFile, IOssEventBase* pEventHandle);

	bool IsDownLoading();
	bool IsExist(tstring& strFullFileName);
protected:
	CPaintManagerUI* m_pPaintManager;
	CTileLayoutGroupPackList* m_pGroupPackFileList;

	//��ǰpacket��������Ϣ
	sPackItem* m_pCurrPackItem;
	bool m_bDownloaded;

	CRITICAL_SECTION m_csFState;
	enum EDownLoadState
	{	
		enm_DownState_OnGoing	= 1,
		enm_DownState_Finish	= 2,
		enm_DownState_Error		= 3,
	};
	map<tstring, EDownLoadState> m_mapFile2State;//���ص��ļ�ȫ��--����״̬
	HWND m_hMainWnd;
	tstring m_strRoomID; //�ĸ�room�����ϰ�
};