#pragma once

#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"

#define WM_SHOW_FRIEND_CHATWND WM_USER+20	//���������л������촰��

class CUIUserInfoPage : public WindowImplBase
{
public:
	CUIUserInfoPage(LPCTSTR pszXMLPath): m_strXmlFile(pszXMLPath){}
	virtual ~CUIUserInfoPage(){};

	//ע�⣺���������const���ͻᱨ˵������Ȼ�����ࡣ
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