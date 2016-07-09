#include "MsgWnd.h"
#include "..\include\MsgWndDefine.h"

CMsgWnd::CMsgWnd()
{

}

CMsgWnd::~CMsgWnd()
{
	PostQuitMessage(0);
}

void CMsgWnd::AddEventHandler(IMsgWndEvent* pEventHandler)
{
	if (m_setMsgWndHandlers.insert(pEventHandler).second == false)
	{
	}
}

void CMsgWnd::RemoveEventHandler(IMsgWndEvent* pEventHandler)
{
	if (!m_setMsgWndHandlers.erase(pEventHandler))
	{
	}
}

LRESULT CMsgWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg > WM_MSG_USER_DEF_BEGIN && uMsg < WM_MSG_USER_DEF_END)
	{
		for (SETMsgWndHandlers::iterator it = m_setMsgWndHandlers.begin();
			it != m_setMsgWndHandlers.end(); ++it)
		{
			IMsgWndEvent* pHandler = *it;
			if (NULL != pHandler)
			{
				pHandler->OnMessage(uMsg, wParam, lParam);
			}
		}
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

CDuiString CMsgWnd::GetSkinFile()
{

	return _T("MsgWnd.xml");
}

CDuiString CMsgWnd::GetSkinFolder()
{
#ifdef _DEBUG
	return _T("..\\MM\\res\\");
#else
	//todo
	return _T("..\\MM\\res\\");
#endif
}

LRESULT CMsgWnd::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
#if defined(WIN32) && !defined(UNDER_CE)
	BOOL bZoomed = ::IsZoomed(m_hWnd);
	LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	if (::IsZoomed(m_hWnd) != bZoomed)
	{

	}
#else
	return __super::OnSysCommand(uMsg, wParam, lParam, bHandled);
#endif

	return 0;
}

void CMsgWnd::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void CMsgWnd::InitWindow()
{}

LRESULT CMsgWnd::ResponseDefaultKeyEvent(WPARAM wParam, LPARAM lPara, bool& bHandled)
{
	if (wParam == VK_RETURN)
	{
		return FALSE;
	}
	else if (wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	return FALSE;
}

UILIB_RESOURCETYPE CMsgWnd::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIP;
#endif
}

CControlUI* CMsgWnd::CreateControl(LPCTSTR pstrClass)
{

	return NULL;
}

LRESULT CMsgWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}


void CMsgWnd::OnExit(TNotifyUI& msg)
{
	Close();
}


LRESULT CMsgWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LPCTSTR CMsgWnd::GetResourceID() const
{
	return MAKEINTRESOURCE(101);
}

LPCTSTR CMsgWnd::GetWindowClassName() const
{
	return _T("MMGui");
}

void CMsgWnd::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		
	}
}