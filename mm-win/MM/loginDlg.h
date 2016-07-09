#pragma once
#include "StdAfx.h"

#include "ControllerEvent_i.h"

#include "Login.h"
#include "registerAccountDlg.h"
#define WM_LOGIN_SUCCESS	WM_USER+101
#define WM_LOGIN_FAILED		WM_USER+102
#define WM_AUTO_LOGIN		WM_USER+103

class loginDlg : public WindowImplBase, IControllerEvent
{
public:

	loginDlg();
	~loginDlg();

public:
	//IControllerEvent
	virtual void OnConnected();
	virtual void OnConnectFail(const int errCode);
	
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void CreateLoginDlg();
	bool NeedUpdate();
	//afx_msg LRESULT OnCreateMainWnd(WPARAM w, LPARAM)
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
	void Notify(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);
protected:
	//登录接口
	void RegisterNewUser();
	void OnBtnLogin();
	void OnResponseAutoLoginMsg();
	void SwitchPasswdMode();
private:
	//CDuiString m_strUser;
	//CDuiString m_strPassword
	bool m_bLogining;
	bool m_bRemeberPwd;
	bool m_bAutoLogin;
	HANDLE m_hEvent;

	bool re_connect_request_;	 //重连请求，
	bool re_connect_response_;  //重连应答，
	static const int WM_TIMER_RECONNECT = 1;
};