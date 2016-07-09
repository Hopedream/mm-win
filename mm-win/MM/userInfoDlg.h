#pragma once
#include "StdAfx.h"



class userInfoDlg : public WindowImplBase
{
public:

	userInfoDlg();
	~userInfoDlg();


public:
	// 
	LPCTSTR GetWindowClassName() const;	
	virtual void OnFinalMessage(HWND hWnd);
	virtual void InitWindow();
	virtual LRESULT ResponseDefaultKeyEvent(WPARAM wParam, LPARAM lPara, bool& bHandled);
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual UILIB_RESOURCETYPE GetResourceType() const;
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LPCTSTR GetResourceID() const;
	// 
	// 	DWORD GetBkColor();
	// 	void SetBkColor(DWORD dwBackColor);
	// 
	// protected:	
	// 
	void Notify(TNotifyUI& msg);
	// 	void OnPrepare(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);
	// 	void OnTimer(TNotifyUI& msg);
	// 
	// private:
	// 
	// 	
	// 

};