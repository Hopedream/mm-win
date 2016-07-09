#pragma once

#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"


#define MB_RETURN_OK	1
#define WM_MEMO_UPDATE WM_USER+33

class CModifyMemoWnd : public WindowImplBase
{
public:
	CModifyMemoWnd();
	virtual ~CModifyMemoWnd();

	virtual LPCTSTR GetWindowClassName()const	{	return _T("ModifyMemoWnd");			}
	virtual CDuiString GetSkinFolder()			{	return _T("..\\MM\\res\\");			}
	virtual CDuiString GetSkinFile()			{	return "modify_partner_memo.xml";	}
	virtual void OnFinalMessage( HWND hWnd )	
	{	
		__super::OnFinalMessage(hWnd);
		delete this;				
	}
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();

	UINT ShowModal(HWND hParnt);
	void ShowWindow(HWND hParent);
	bool InitMemo(tstring& strAccount, tstring& strMemo);

public:
	tstring m_strNewMemo;

protected:
	tstring m_strCurMemo;
	tstring m_strAccount;
	HWND m_hMainWnd;
};