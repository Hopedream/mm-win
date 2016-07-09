#pragma once

#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"

#define WM_SHOW_FRIEND_CHATWND WM_USER+20	//让主界面切换到聊天窗口

class CUIUserInfoPage : public WindowImplBase
{
public:
	CUIUserInfoPage(LPCTSTR pszXMLPath): m_strXmlFile(pszXMLPath){}
	virtual ~CUIUserInfoPage(){};

	//注意：如果不加上const，就会报说该类仍然是虚类。
	virtual LPCTSTR GetWindowClassName()const	{	return _T("UserInfoPage");	}
	virtual CDuiString GetSkinFolder()			{	return (_T(""));				}
	virtual CDuiString GetSkinFile()			{	return m_strXmlFile;		}
	virtual void OnFinalMessage( HWND hWnd )	{	delete this;				}
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);


	void CreateWnd(HWND& hParentWnd, POINT ptPos);
	bool Initialize(tstring& strAccount, tstring strAvatar);

protected:
	HWND m_hWndParent;
	CDuiString m_strXmlFile;
	tstring m_strCurAccount;
};