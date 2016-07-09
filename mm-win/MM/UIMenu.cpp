#include "UIMenu.h"
#include "../include/Global_Def.h"
#include "videoDlg.h"
#include "ChatAndFilePage.h"
#include "..\Common\MMDefine.h"
#include "GroupChatPage.h"
#include "mainDlg.h"

//DUI_BEGIN_MESSAGE_MAP(CDuiMenu, WindowImplBase)
//	DUI_ON_MSGTYPE(DUI_MSGTYPE_CLICK,OnClick)
//	DUI_END_MESSAGE_MAP()

//			::PostMessage(m_hParentWnd, WM_REMOVE_USER,0,0);
//			//用完记得close窗口
//			Close();

#define BTNOPT_ENTER_SEND_MODE "btnOptEntreSend"
#define BTNOPT_CTRL_ENTER_SEND_MODE "btnOptCtrlEntreSend"

#define BLUE_COLOR 0XFF21A7D4
#define BLACK_COLOR 0XFF000000
#define WHITE_COLOR 0XFFFFFFFF

CDuiMenu::CDuiMenu( LPCTSTR pszXMLPath , eContextMenu _contextMenuType) 
	: CXMLWnd(pszXMLPath), m_eContextMenuType(_contextMenuType)
{
	m_hParentWnd = NULL;
	m_pRichEditUI = NULL;
}

CDuiMenu::~CDuiMenu()
{

}

void CDuiMenu::Init( HWND hWndParent, POINT ptPos )
{
	m_hParentWnd = hWndParent;

	Create(hWndParent, _T("MenuWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);

	if (eUIMenuSysSetting == m_eContextMenuType) //设计图要求弹出菜单和主窗口框架底部对其，无奈之下做此调整。--llzhou
	{
		RECT rect;
		GetClientRect(this->GetHWND(), &rect);
		ptPos.y -= (rect.bottom - rect.top);
		ptPos.y -= 5;
	}

	::ClientToScreen(hWndParent, &ptPos);	
	::SetWindowPos(m_hWnd, NULL, ptPos.x, ptPos.y, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
}



LRESULT CDuiMenu::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT lRes = 0;
	BOOL bHandled = TRUE;

	switch( uMsg )
	{
	case WM_KILLFOCUS:
		lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); 
		break; 
	default:
		bHandled = FALSE;
	}

	if(bHandled || m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) 
	{
		return lRes;
	}

	if (m_PaintManager.MessageHandler(uMsg, wParam, lParam, lRes)) 
	{
		return lRes;
	}

	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CDuiMenu::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	Close();
	bHandled = FALSE;
	return __super::OnKillFocus(uMsg, wParam, lParam, bHandled); 
}



void CDuiMenu::HandleCallTypeContextMenuEvent(TNotifyUI& msg)
{
	
	if (_tcsicmp(msg.pSender->GetName(), "callIpPhone")==0)
	{	
		CVideoDlg::Instance()->PostMessage( WM_CALL_IP_PHONE, 0, 0);
	}
	else if (_tcsicmp(msg.pSender->GetName(), "callSmartPhone")==0)
	{	
		CVideoDlg::Instance()->PostMessage( WM_CALL_SMART_PHONE, 0, 0);
	}
	else if (_tcsicmp(msg.pSender->GetName(), "callBack")==0)
	{	
		CVideoDlg::Instance()->PostMessage( WM_CALL_BACK, 0, 0);
	}
}

void CDuiMenu::HandleSysSettingMenuEvent( TNotifyUI& msg )
{
	
	if (_tcsicmp(msg.pSender->GetName(), "btn_change_password") == 0)
	{
		::PostMessage(m_hParentWnd, WM_SYS_CHANGE_PASSWORD, 0, 0);
	}
	else if (_tcsicmp(msg.pSender->GetName(), "btn_create_org") == 0)
	{
		::PostMessage(m_hParentWnd, WM_SYS_CREATE_ORG, 0, 0);
	}
	else if (_tcsicmp(msg.pSender->GetName(), "btn_change_user_info") == 0)
	{
		::PostMessage(m_hParentWnd, WM_SYS_USER_INFO, 0, 0);
	}
	else if (_tcsicmp(msg.pSender->GetName(), "btn_org_info") == 0)
	{
		::PostMessage(m_hParentWnd, WM_SYS_ORG_INFO, 0, 0);
	}
	else if (_tcsicmp(msg.pSender->GetName(), "btn_change_organization_info") == 0)
	{
		::PostMessage(m_hParentWnd, WM_SYS_CHANGE_ORGANIZATION_INFO, 0, 0);
	}
	else if (_tcsicmp(msg.pSender->GetName(), "button_employee_manage") == 0)
	{
		::PostMessage(m_hParentWnd, WM_SYS_EMPLOYEE_MANAGE, 0, 0);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("button_reset")) == 0)
	{
		::PostMessage(m_hParentWnd, WM_SYS_RESET_PASSWORD, 0, 0);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("button_about")) == 0)
	{
		::PostMessage(m_hParentWnd, WM_SYS_ABOUT, 0, 0);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("button_log_off")) == 0)
	{
		::PostMessage(m_hParentWnd, WM_SYS_LOG_OFF, 0, 0);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("button_quit")) == 0)
	{
		::PostMessage(m_hParentWnd, WM_SYS_QUIT, 0, 0);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("btn_quit_org")) == 0)
	{
		::PostMessage(m_hParentWnd, WM_SYS_QUIT_ORG, 0, 0);
	}
}

void CDuiMenu::HandleSendMsgKeyMenuEvent( TNotifyUI& msg )
{	
	if (_tcsicmp(msg.pSender->GetName(), BTNOPT_ENTER_SEND_MODE) == 0)
	{
		CChatAndFilePage::Instance()->SetSendMsgKeyMode(eEnterKey);
		mainDlg::getInstance()->GetGroupPage()->SetSendMsgKeyMode(eEnterKey);
	}
	else if (_tcsicmp(msg.pSender->GetName(), BTNOPT_CTRL_ENTER_SEND_MODE) == 0)
	{
		CChatAndFilePage::Instance()->SetSendMsgKeyMode(eCtrlEnterKey);
		mainDlg::getInstance()->GetGroupPage()->SetSendMsgKeyMode(eCtrlEnterKey);
	}	
}

void CDuiMenu::HandleKillFocusEvent(TNotifyUI& msg)
{

}

void CDuiMenu::HandleSetFocusEvent(TNotifyUI& msg)
{
	
}

void CDuiMenu::HandleClickEvent( TNotifyUI& msg )
{
	if (FALSE == ::IsWindow(m_hParentWnd))
	{
		return;
	}

	if (eUIMenuDeleteFriend == m_eContextMenuType)
	{
		if (_tcsicmp(msg.pSender->GetName(), "buttonMoveFriend")==0)
		{

		}
		//下面这几个消息在PartnerInfoPage中均已经实现，因此要想办法调用
		else if (_tcsicmp(msg.pSender->GetName(), "buttonSendMsg")==0)
		{
			if (::IsWindow(m_hParentWnd))
			{
				::PostMessage(m_hParentWnd, WM_SEND_MSG, 0, 0);
			}		
		}
		else if (_tcsicmp(msg.pSender->GetName(), "buttonNetPhone")==0)
		{
			if (::IsWindow(m_hParentWnd))
			{
				::PostMessage(m_hParentWnd, WM_NET_PHONE, 0, 0);
			}		
		}
		else if (_tcsicmp(msg.pSender->GetName(), "buttonMobilePhone")==0)
		{
			if (::IsWindow(m_hParentWnd))
			{
				::PostMessage(m_hParentWnd, WM_MOBILE_PHONE, 0, 0);
			}		
		}
		else if (_tcsicmp(msg.pSender->GetName(), "buttonBusiness")==0)
		{
			if (::IsWindow(m_hParentWnd))
			{
				::PostMessage(m_hParentWnd, WM_BUSINESS, 0, 0);
			}		
		}
		else if (_tcsicmp(msg.pSender->GetName(), "buttonDeleteFriend")==0)
		{
			if (::IsWindow(m_hParentWnd))
			{
				::PostMessage(m_hParentWnd, WM_REMOVE_USER, 0, 0);
			}			
		}
		else if (_tcsicmp(msg.pSender->GetName(), "buttonDeleteContact")==0)
		{
			if (::IsWindow(m_hParentWnd))
			{
				::PostMessage(m_hParentWnd, WM_REMOVE_CONTACT, 0, 0);
			}			
		}
		else 
		{

		}
	}
	else if(eUIMenuCallTypeDrop == m_eContextMenuType)
	{
		HandleCallTypeContextMenuEvent(msg);
	}	
	else if(eUIMenuSysSetting == m_eContextMenuType)
	{
		HandleSysSettingMenuEvent(msg);
	}
	else if (eUIMenuSendMsgKeyDrop == m_eContextMenuType)
	{
		HandleSendMsgKeyMenuEvent(msg);
	}
	else if (eUIMenuRichEditContextMenu == m_eContextMenuType)
	{
		HandleRichEditContextMenu(msg);
	}

	Close();
}

void CDuiMenu::HandleWindowInitEvent( TNotifyUI& msg )
{
	if (eUIMenuSendMsgKeyDrop == m_eContextMenuType)
	{
		CControlUI* pCtrlOptEnter = static_cast<CControlUI*>(m_PaintManager.FindControl("optEntrlSend"));
		CControlUI* pCtrlOptCtrlAndEnter = static_cast<CControlUI*>(m_PaintManager.FindControl("optCtrlEntrlSend"));
		CButtonUI* pBtnOptEnter = static_cast<CButtonUI*>(m_PaintManager.FindControl(BTNOPT_ENTER_SEND_MODE));
		CButtonUI* pBtnOptCtrlAndEnter = static_cast<CButtonUI*>(m_PaintManager.FindControl(BTNOPT_CTRL_ENTER_SEND_MODE));

		if (CChatAndFilePage::Instance()->IsEnterSendMode()
			||((CGroupChatPage*)(mainDlg::getInstance()->GetGroupPage()))->IsEnterSendMode())
		{
			pCtrlOptEnter->SetBkImage("send_menu_selected.png");
			pCtrlOptEnter->SetBkColor(BLUE_COLOR);
			pBtnOptEnter->SetBkColor(BLUE_COLOR);
			pBtnOptEnter->SetTextColor(WHITE_COLOR);		
		}
		else
		{
			pCtrlOptCtrlAndEnter->SetBkImage("send_menu_selected.png");
			pCtrlOptCtrlAndEnter->SetBkColor(BLUE_COLOR);
			pBtnOptCtrlAndEnter->SetBkColor(BLUE_COLOR);
			pBtnOptCtrlAndEnter->SetTextColor(WHITE_COLOR);
		}
	}
	else if (eUIMenuMsgEmpty == m_eContextMenuType)
	{
		(static_cast<CButtonUI*>(m_PaintManager.FindControl("btnMsg")))->SetText("不能发送空白消息");		
	}
	else if (eUIMenuMsgTooLog == m_eContextMenuType)
	{
		(static_cast<CButtonUI*>(m_PaintManager.FindControl("btnMsg")))->SetText("发送消息内容超长");		
	}
	else if (eUIMenuRichEditContextMenu == m_eContextMenuType && m_pRichEditUI)
	{
		(static_cast<CButtonUI*>(m_PaintManager.FindControl("richedit_cut")))->SetEnabled(!m_pRichEditUI->GetSelText().IsEmpty());
		(static_cast<CButtonUI*>(m_PaintManager.FindControl("richedit_copy")))->SetEnabled(!m_pRichEditUI->GetSelText().IsEmpty());
	}
}

void CDuiMenu::HandleRichEditContextMenu(TNotifyUI& msg)
{
	if (NULL == m_pRichEditUI)
	{
		return;
	}

	CDuiString strCtrlName = msg.pSender->GetName();
	if (_tcsicmp(strCtrlName, "richedit_cut") == 0)
	{
		m_pRichEditUI->Cut();
	}
	else if (_tcsicmp(strCtrlName, "richedit_copy") == 0)
	{
		m_pRichEditUI->Copy();
	}
	else if (_tcsicmp(strCtrlName, "richedit_paste") == 0)
	{
		m_pRichEditUI->Paste();
	}	
}



void CDuiMenu::SetRichEditUI( CRichEditUI* pRichEditUI )
{
	m_pRichEditUI = pRichEditUI;
}

// void CDuiMenu::Notify(TNotifyUI& msg)   //控件消息响应
// {
// 	int aaa = 1;
// 	aaa++;
// 	int bb =2;
// }
