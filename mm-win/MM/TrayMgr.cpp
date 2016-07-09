#include "StdAfx.h"
#include "traymgr.h"
#include <algorithm>


enum
{
	eTimerId_TrayFlash = 0,
	eTimerInterval_TrayFlash = 250,

	eTimerId_CheckTrayDblClk = 1,
	eTimerInterval_CheckTrayDblClk = 300,
};

enum
{
	WM_DefTrayCallbackMsgId = WM_USER + 1001,
};

//消息窗口类
static const TCHAR WndClassForTrayWnd[]  = _T("WndClassForTrayWnd_MM");
//全局变量
static CTrayMgr* g_pThis = NULL;

CTrayMgr::CTrayMgr()
{
	memset(&m_nid, 0, sizeof m_nid);
	m_bFlashing = FALSE;
	m_hwndTray = NULL;

	m_eTNMode = eTNMode_Null;
	m_bWaitForDblClk = FALSE;
	m_eTFSMode = eTFSMode_DoubleClick;
	m_hiconBeforeFlash = NULL;
	m_uTrayFlashTimerId = -1;
}

CTrayMgr::~CTrayMgr(void)
{
	if (m_nid.cbSize)
	{
		Delete();
	}

	DestroyWindow(m_hwndTray);
	m_hwndTray = NULL;
}

//消息窗口函数
LRESULT CTrayMgr::TrayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == WM_DefTrayCallbackMsgId)
	{
		if (lParam == WM_LBUTTONDOWN)
		{
			SetTimer(hWnd, eTimerId_CheckTrayDblClk, eTimerInterval_CheckTrayDblClk, NULL);
			g_pThis->m_bWaitForDblClk = TRUE;
		
			if (g_pThis->m_bFlashing)
			{
				if (g_pThis->m_eTFSMode == eTFSMode_Click)
				{	
					g_pThis->FireOnTrayEvent(g_pThis->m_stackFlash.back().dwParam, WM_TrayFlashStopped);
				}
			}
			else
			{
				g_pThis->FireOnTrayEvent(wParam, lParam);
			}
		}

		//注意，这里按照测试部的要求，单击就得有响应
		else if (lParam == WM_LBUTTONDBLCLK)
		{
			KillTimer(hWnd, eTimerId_CheckTrayDblClk);
			g_pThis->m_bWaitForDblClk = FALSE;

			if (g_pThis->m_bFlashing)
			{
				if (g_pThis->m_eTFSMode == eTFSMode_DoubleClick)
				{
					
					g_pThis->FireOnTrayEvent(g_pThis->m_stackFlash.back().dwParam, WM_TrayFlashStopped);
					
				}
			}
			else
			{
				g_pThis->FireOnTrayEvent(wParam, lParam);
			}
		}
		else if (lParam == WM_LBUTTONDOWN_Late || lParam == WM_RBUTTONDOWN || lParam == WM_RBUTTONUP)
		{
			g_pThis->FireOnTrayEvent(wParam, lParam);
		}
		else
		{
			////TRACE(_T(""));
		}
	}
	else if (message == WM_TIMER)
	{
		UINT nIDEvent = wParam;

		if (nIDEvent == eTimerId_CheckTrayDblClk)
		{
			//by zyy 不需要双击就直接响应
			if (g_pThis->m_bWaitForDblClk)
			{
				KillTimer(hWnd, nIDEvent);
				g_pThis->m_bWaitForDblClk = FALSE;

				SendMessage(hWnd, WM_DefTrayCallbackMsgId, 0, WM_LBUTTONDOWN_Late);

				//这里由zyy添加 即单击响应后，再人为的制造一个双击的事件
				SendMessage(hWnd, WM_DefTrayCallbackMsgId, 0, WM_LBUTTONDBLCLK);
			}
		}
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

ATOM CTrayMgr::RegisterClassForTrayWnd(/*HINSTANCE hInstance*/)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= 0/*CS_HREDRAW | CS_VREDRAW;*/;
	wcex.lpfnWndProc	= (WNDPROC)TrayWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= NULL/*hInstance*/;
	wcex.hIcon			= NULL;
	wcex.hCursor		= NULL;
	wcex.hbrBackground	= NULL;//(HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)NULL;
	wcex.lpszClassName	= WndClassForTrayWnd;
	wcex.hIconSm		= NULL;//LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

/**
* 初始化任务栏的提醒按钮
*/
void CTrayMgr::Init(HWND hwndAttach/* = No_Care_HWND*/, ETrayNotifyMode eTNMode/* = eTNMode_Event*/, ETrayFlashStopMode eTFSMode/* = eTFSMode_DoubleClick*/)
{
	m_eTNMode = eTNMode;
	m_eTFSMode = eTFSMode;

	if (eTNMode == eTNMode_Event)
	{
		if (!g_pThis)
		{
			g_pThis = this;
		}

		//ASSERT(hwndAttach == No_Care_HWND);

		//注册消息窗口
		RegisterClassForTrayWnd();

		//创建消息窗口
		m_hwndTray = CreateWindow(WndClassForTrayWnd, NULL, WS_POPUP, 0, 0, 0, 0, NULL, NULL, NULL, NULL);
		//ASSERT(m_hwndTray);

		//指定了任务栏Icon接收消息的窗口
		m_nid.hWnd = m_hwndTray;
	}
	else
	{
		//ASSERT(hwndAttach != No_Care_HWND);
		//ASSERT(hwndAttach);

		m_nid.hWnd = hwndAttach;
	}
}

BOOL CTrayMgr::AddTaskbarIcon(HICON hicon, LPCTSTR szTip, UINT uCallbackMsgId/* = No_Care*/)
{
	BOOL bRet = FALSE;

	//ASSERT(m_nid.hWnd);
	//ASSERT(!m_bFlashing);

	if (!m_nid.cbSize)
	{// no exist yet
		m_nid.cbSize = sizeof m_nid;
		m_nid.uFlags |= NIF_TIP | NIF_ICON | NIF_MESSAGE;
		m_nid.hIcon = hicon;
		m_nid.uID = 0;
		if (uCallbackMsgId == No_Care)
		{
			//ASSERT(m_eTNMode == eTNMode_Event);
			m_nid.uCallbackMessage = WM_DefTrayCallbackMsgId;
		}
		else
		{
			m_nid.uCallbackMessage = uCallbackMsgId;
		}
		_tcsncpy(m_nid.szTip, szTip, sizeof m_nid.szTip);

		Shell_NotifyIcon(NIM_ADD, &m_nid);
	}
	else
	{// exist already
		Change(hicon, szTip, uCallbackMsgId);
	}
	
	return bRet;
}

BOOL CTrayMgr::InnerChange(HICON hicon/* = Use_Current_HICON*/, LPCTSTR szTip/* = Use_Current_SZ*/, UINT uCallbackMsgId/* = Use_Current*/)
{
	BOOL bRet = FALSE;
	
	//ASSERT(m_nid.cbSize && m_nid.hWnd);

	BOOL bChangeIcon = (hicon != Use_Current_HICON);
	BOOL bChangeMsgId = (uCallbackMsgId != Use_Current);
	BOOL bChangeTip = (szTip != Use_Current_SZ);

	//m_nid.uFlags |= (bChangeIcon ? NIF_ICON : 0) | (bChangeTip ? NIF_TIP : 0) | (bChangeMsgId ? NIF_MESSAGE : 0);
	
	if (bChangeIcon)
	{
		m_nid.hIcon = hicon;
	}
	if (bChangeMsgId)
	{
		m_nid.uCallbackMessage = uCallbackMsgId;
	}
	if (bChangeTip)
	{
		_tcscpy(m_nid.szTip, szTip);
	}

	Shell_NotifyIcon(NIM_MODIFY, &m_nid);

	return bRet;
}

BOOL CTrayMgr::Delete()
{
	//ASSERT(!m_bFlashing);
	//移除任务栏的Icon图标
	Shell_NotifyIcon(NIM_DELETE, &m_nid);
	memset(&m_nid, 0, sizeof m_nid);

	return TRUE;
}

//闪烁效果。
BOOL CTrayMgr::Flash(HICON hiconNew/* = Use_Current_HICON*/, DWORD dwParam/* = 0*/)
{
	if (!g_pThis)
	{
		g_pThis = this;
	}
	
	if (hiconNew == Use_Current_HICON && !m_nid.hIcon)
	{
		//ASSERT(0);
		return FALSE;
	}

	if (hiconNew != Use_Current_HICON && !m_nid.cbSize)
	{
		//ASSERT(0);
		return FALSE;
	}

	STrayFlashStackItem item;
	item.hicon = hiconNew == Use_Current_HICON ? m_nid.hIcon : hiconNew;
	item.dwParam = dwParam;
	m_stackFlash.push_back(item);

	if (!m_bFlashing)
	{
		m_uTrayFlashTimerId = SetTimer(NULL, eTimerId_TrayFlash, eTimerInterval_TrayFlash, CTrayMgr::OnFlashTimer);
		m_bFlashing = TRUE;

		m_hiconBeforeFlash = m_nid.hIcon;	// record it
	}

	return TRUE;
}

void CALLBACK CTrayMgr::OnFlashTimer(HWND hwnd, UINT uMsg, UINT nEvent, DWORD dwTime)
{
	static BOOL bToogle = TRUE;

	if (g_pThis->m_bFlashing)
	{
		g_pThis->ChangeIcon(bToogle ? g_pThis->m_stackFlash.back().hicon : NULL);
		bToogle = !bToogle;
	}
	else
	{
		//ASSERT(0);
	}
}

BOOL CTrayMgr::StopFlash(int nIndex/* = -1*/)
{
	if (!m_stackFlash.empty())
	{
		if (nIndex == -1 || nIndex == m_stackFlash.size() - 1)
		{
			m_stackFlash.pop_back();

			if (!m_stackFlash.size())
			{
				KillTimer(NULL, m_uTrayFlashTimerId);
				m_bFlashing = FALSE;

				if (m_hiconBeforeFlash)
				{
					ChangeIcon(m_hiconBeforeFlash);
				}
			}
		}
		else
		{
			m_stackFlash.erase(m_stackFlash.begin() + nIndex);
		}
	}
	else
	{
		//ASSERT(0);
	}

	return TRUE;
}

BOOL CTrayMgr::StopAllFlash()
{
	if (!m_stackFlash.empty())
	{
		KillTimer(NULL, m_uTrayFlashTimerId);

		while (!m_stackFlash.empty())
		{
			m_stackFlash.pop_back();
		}

		m_bFlashing = FALSE;

		if (m_hiconBeforeFlash)
		{
			ChangeIcon(m_hiconBeforeFlash);
		}
	}
	else
	{
		//ASSERT(0);
	}

	return TRUE;
}

void CTrayMgr::AddEventHandler(ITrayEvent* pHandler)
{
	if (m_vecTrayEventHandler.empty() ||
		find(m_vecTrayEventHandler.begin(), m_vecTrayEventHandler.end() -1, pHandler) == m_vecTrayEventHandler.end()
		)
	{
		m_vecTrayEventHandler.push_back(pHandler);
	}
	
}

void CTrayMgr::DeleteEventHandler(ITrayEvent* pHandler)
{
	if (!m_vecTrayEventHandler.empty())
	{
		vector<ITrayEvent*>::iterator it = find(m_vecTrayEventHandler.begin(), m_vecTrayEventHandler.end() -1, pHandler);
		if (it != m_vecTrayEventHandler.end())
		{
			m_vecTrayEventHandler.erase(it);
		}
		else
		{
			//ASSERT(0);
		}
	}
	else
	{
		//ASSERT(0);
	}
}

void CTrayMgr::FireOnTrayEvent(WPARAM w, LPARAM l)
{
	int size = m_vecTrayEventHandler.size();
	for (vector<ITrayEvent*>::iterator it = m_vecTrayEventHandler.begin(); it != m_vecTrayEventHandler.end(); it++)
	{
		(*it)->OnTrayEvent(w, l);
	}
}
HWND CTrayMgr::GetWindowHandle()
{
	return m_hwndTray;
}

DWORD CTrayMgr::GetParam(int idx)
{
	//ASSERT(idx < m_stackFlash.size());
	STrayFlashStackItem& r = m_stackFlash.at(idx);
	return r.dwParam;
}