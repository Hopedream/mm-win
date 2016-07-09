#pragma once
#include "StdAfx.h"
#include <windows.h>
#include <set>

using namespace std;

class IMsgWndEvent
{
public:
	virtual void OnMessage(UINT uMsg, WPARAM w, LPARAM l) = 0;
};

class CMsgWnd : public WindowImplBase
{
public:
	CMsgWnd();
	virtual ~CMsgWnd();
	void AddEventHandler(IMsgWndEvent* pEventHandler);
	void RemoveEventHandler(IMsgWndEvent* pEventHandler);

public:
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
	void Notify(TNotifyUI& msg);
	void OnExit(TNotifyUI& msg);


	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	typedef std::set<IMsgWndEvent*> SETMsgWndHandlers;
	SETMsgWndHandlers m_setMsgWndHandlers;
};