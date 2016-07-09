#pragma once
#include "StdAfx.h"
#include "resource.h"
#include "MMWinImplBase.h"
#include "../include/Global_Def.h"
#include "UIMenu.h"


void MMWindowImplBase::HandleNotifyEvent(TNotifyUI& msg)
{
	if (0 == _tcsicmp(DUI_MSGTYPE_SETFOCUS, msg.sType))
	{
		HandleSetFocusEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_KILLFOCUS, msg.sType))
	{
		HandleKillFocusEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_BUTTONDOWN, msg.sType))
	{
		HandleButtonDownEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_TEXTCHANGED, msg.sType))
	{
		HandleTextChangedEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_SELECTCHANGED, msg.sType))
	{
		HandleSelectedChangedEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_VALUECHANGED, msg.sType))
	{
		HandleValueChangedEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_ITEMSELECT, msg.sType))
	{
		HandleItemSelectEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_ITEMCLICK, msg.sType))
	{
		HandleItemClickEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_ITEMDBCLICK, msg.sType))
	{
		HandleItemDBClickEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_TABSELECT, msg.sType))
	{
		HandleTabSelectEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_ITEMEXPAND, msg.sType))
	{
		HandleItemExpandEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_WINDOWINIT, msg.sType))
	{
		HandleWindowInitEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_MOUSEENTER, msg.sType))
	{
		HandleMouseEnterEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_MOUSELEAVE, msg.sType))
	{
		HandleMouseLeaveEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_HEADERCLICK, msg.sType))
	{
		HandleHeaderClickEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_MENU, msg.sType))
	{
		HandleMenuEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_LINK, msg.sType))
	{
		HandleLinkEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_TIMER, msg.sType))
	{
		HandleTimerEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_RETURN, msg.sType))
	{
		HandleReturnEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_SCROLL, msg.sType))
	{
		HandleScrollEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_DROPDOWN, msg.sType))
	{
		HandleDropdownEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_SHOWACTIVEX, msg.sType))
	{
		HandleShowActivXEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_ITEMCOLLAPSE, msg.sType))
	{
		HandleItemCollapseEvent(msg);
	}
	else if (0 == _tcsicmp(DUI_MSGTYPE_ITEMACTIVATE, msg.sType))
	{
		HandleItemAtivateEvent(msg);
	}
}

void MMWindowImplBase::Notify( TNotifyUI& msg )
{
	if ( _tcscmp(msg.sType, DUI_MSGTYPE_RICHEDIT_CONTEXT_MENU) == 0 )  
	{  		
		HandleRichEditContextMenu(msg);
	}  
	__super::Notify(msg);
	HandleNotifyEvent(msg);	
}

LPCTSTR MMWindowImplBase::GetWindowClassName( void ) const
{	
	return _T("MMGui");	
}

void MMWindowImplBase::OnClick( TNotifyUI& msg )
{
	WindowImplBase::OnClick(msg);
	HandleClickEvent(msg);
}

void MMWindowImplBase::OnFinalMessage( HWND hWnd )
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}



LRESULT MMWindowImplBase::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	Close();
	bHandled = FALSE;
	return 0;
}

DuiLib::UILIB_RESOURCETYPE MMWindowImplBase::GetResourceType() const
{
#ifdef _DEBUG
	return UILIB_FILE;
#else
	return UILIB_ZIP;
#endif
}

DuiLib::CDuiString MMWindowImplBase::GetSkinFolder()
{
#ifdef _DEBUG
	return _T("..\\MM\\res\\");
#else
	//todo
	return _T("..\\MM\\res\\");
#endif
}


void MMWindowImplBase::CreateWnd(HWND hWnd)
{	
	Create(hWnd, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);	
	CenterWindow();
	ShowWindow(true);
}

void MMWindowImplBase::SetTopMostWnd(bool bTop)
{
	RECT rc;
	GetWindowRect(this->GetHWND(), &rc);
	::SetWindowPos(*this, bTop ? HWND_TOPMOST : HWND_NOTOPMOST,rc.left, rc.top, 0, 0,  SWP_NOSIZE | SWP_NOACTIVATE);
}

LRESULT MMWindowImplBase::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if (WM_TIMER == uMsg )
	{
		if (wParam == WM_TIMER_TOPWND)
		{
			::SetWindowPos(this->GetHWND(), HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			KillTimer(this->GetHWND(), WM_TIMER_TOPWND);
		}
		else if (wParam == WM_TIMER_TOOLTIP_BOX_HIDE)
		{
			KillTimer(this->GetHWND(), WM_TIMER_TOOLTIP_BOX_HIDE);
			Close();
		}
		
	}
	if(uMsg == WM_NCLBUTTONDBLCLK && HTCAPTION==wParam)
	{
		return 0;
	}
	return __super::HandleMessage(uMsg, wParam, lParam);
}

void MMWindowImplBase::SetForegroundWnd()
{
	::SetWindowPos(this->GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	SetTimer(this->GetHWND(), WM_TIMER_TOPWND, 50, NULL);
}

void MMWindowImplBase::ShowToolTipBox()
{
	ShowWindow(true);
	SetTimer(this->GetHWND(), WM_TIMER_TOOLTIP_BOX_HIDE, 500, NULL);
}

void MMWindowImplBase::HandleRichEditContextMenu( TNotifyUI& msg )
{
	POINT pt;
	pt.x = msg.ptMouse.x;
	pt.y = msg.ptMouse.y;

	CDuiMenu *pMenu = new CDuiMenu(_T("menu_richedit_contextmenu.xml"), eUIMenuRichEditContextMenu);
	pMenu->SetRichEditUI((CRichEditUI*)msg.pSender);
	pMenu->Init(*this, pt);
	pMenu->ShowWindow(TRUE);
}

