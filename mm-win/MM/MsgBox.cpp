#include "StdAfx.h"
#include "MsgBox.h"


CMsgBox::CMsgBox(const tstring strTile, const tstring strText, UINT icon, UINT uMsgBoxType)
{
	m_pFlagCtrl = NULL;
	m_pText = NULL;
	m_pTile = NULL;
	m_pBnYes = NULL;
	m_pBnNO  = NULL;
	m_iconIdx = icon;
	m_strTitle = strTile;
	m_strText  = strText;
	m_msgboxType = uMsgBoxType;	
}


UINT CMsgBox::ShowModal(HWND hParent)
{
	hParent = GetWindowOwner(hParent) ? GetWindowOwner(hParent): hParent;
	Create(hParent, _T(""), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE , 0, 0, 0, 0, NULL);
	CenterWindow();
	SetTopMostWnd();
	return  CWindowWnd::ShowModal();
}


void CMsgBox::ShowWindow(HWND hParent)
{	
	Create(hParent, _T("MsgBox"), WS_EX_TOOLWINDOW | WS_EX_TOPMOST, WS_EX_TOOLWINDOW, 0, 0, 398,200);
	CenterWindow();
	return CWindowWnd::ShowWindow();
}


void CMsgBox::InitWindow()
{
	m_pFlagCtrl = static_cast<CControlUI*>(m_PaintManager.FindControl("FlagCtrl"));
	m_pText = static_cast<CControlUI*>(m_PaintManager.FindControl("TxtTips"));
	m_pTile = static_cast<CControlUI*>(m_PaintManager.FindControl("LabelTitle"));	

	m_pBnOK = static_cast<CControlUI*>(m_PaintManager.FindControl("BnOK"));
	m_pBnYes = static_cast<CControlUI*>(m_PaintManager.FindControl("BnYes"));
	m_pBnNO  = static_cast<CControlUI*>(m_PaintManager.FindControl("BnNo"));	

	if ( NULL != m_pBnOK ) m_pBnOK->SetVisible(false);
	if ( NULL != m_pBnYes ) m_pBnYes->SetVisible(false);
	if ( NULL != m_pBnNO ) m_pBnNO->SetVisible(false);
	m_pBnOK->SetText("确定");
	m_pBnYes->SetText("确定");
	m_pBnNO->SetText("取消");
}

void CMsgBox::HandleClickEvent( TNotifyUI& msg )
{
	int nRetCode;
	 if ( msg.pSender == m_pBnYes)
	{
		nRetCode = MSGBOX_IDYES;
	}
	else if (msg.pSender == m_pBnOK)
	{
		nRetCode = MSGBOX_IDOK;
	}
	else if ( msg.pSender = m_pBnNO )
	{
		nRetCode = MSGBOX_IDNO;
	}

	PostMessage(WM_CLOSE, (WPARAM)nRetCode);
}

void CMsgBox::HandleWindowInitEvent( TNotifyUI& msg )
{
	CDuiString strIcon; 
	strIcon.Format("systemMsgBox\\MsgIcon%d.PNG", m_iconIdx);
	m_pFlagCtrl->SetBkImage(strIcon);

	m_pText->SetText(m_strText.c_str());
	m_pTile->SetText(m_strTitle.c_str());

	m_pBnOK->SetVisible(m_msgboxType&MSGBOX_IDOK);
	m_pBnYes->SetVisible(m_msgboxType&MSGBOX_IDYES);
	m_pBnNO->SetVisible(m_msgboxType&MSGBOX_IDNO);	
}

int CMsgBox::MsgBox( HWND hParen, const tstring strTile /*= ""*/, const tstring strText /*= ""*/, UINT icon /*= MSGBOX_ICON_NORMAL*/, UINT uMsgBoxType /*= MSGBOX_IDOK*/ )
{
	CMsgBox* lpMsgBox = new CMsgBox(strTile, strText, icon, uMsgBoxType);
	return lpMsgBox->ShowModal(hParen);
}
