#pragma once

#include "StdAfx.h"

class CMessageBox : public WindowImplBase
{
public:
	CMessageBox();
	virtual ~CMessageBox();

	void SetMainWndHWND(HWND hWndMaind)
	{
		m_hMainWnd = hWndMaind;
	}

	//注意：如果不加上const，就会报说该类仍然是虚类。
	virtual LPCTSTR GetWindowClassName()const	{	return _T("MMMessageBox");			}
	virtual CDuiString GetSkinFolder()			{	return _T("..\\MM\\res\\");				}
	virtual CDuiString GetSkinFile()			{	return "MsgBoxMM.xml";		}
	virtual void OnFinalMessage( HWND hWnd )	{	delete this;				}
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();

	void CreateWnd();
	bool Initialize(tstring& strTips);
	
protected:
	tstring m_strCurAccount;
	HWND m_hMainWnd;
};