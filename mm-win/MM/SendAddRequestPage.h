#pragma once

#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"
#include "MMWinImplBase.h"


class CSendAddRequestPage : public MMWindowImplBase
{
public:
	CSendAddRequestPage();
	virtual ~CSendAddRequestPage();

	void SetMainWndHWND(HWND hWndMaind)
	{
		m_hMainWnd = hWndMaind;
	}

	//ע�⣺���������const���ͻᱨ˵������Ȼ�����ࡣ
	virtual LPCTSTR GetWindowClassName()const	{	return _T("SendAddPartnerPage");			}
	virtual CDuiString GetSkinFile()			{	return "send_addpartner_request_page.xml";		}
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	//virtual void handle
	//void CreateWnd();
	bool Initialize(sUserVCard* pUserInfo);

protected:
	tstring m_strCurAccount;
	HWND m_hMainWnd;
};