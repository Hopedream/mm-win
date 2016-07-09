#pragma once

#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"

class CViewFoundPartnerInfoPage : public WindowImplBase
{
public:
	CViewFoundPartnerInfoPage(WindowImplBase* pParent=NULL);
	virtual ~CViewFoundPartnerInfoPage();

	virtual LPCTSTR GetWindowClassName()const	{	return _T("ViewFoundPartnerInfoPage");			}
	virtual CDuiString GetSkinFolder()			{	return _T("..\\MM\\res\\");						}
	virtual CDuiString GetSkinFile()			{	return "view_bussiness_partner_page.xml";		}
	virtual void OnFinalMessage( HWND hWnd )	{	delete this;				}
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	
	void CreateWnd();
	bool Initialize(sUserVCard* pUserInfo);

protected:
	tstring m_strCurAccount;
	WindowImplBase* m_pParentWnd;
};