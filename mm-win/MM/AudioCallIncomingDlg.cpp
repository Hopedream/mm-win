#include "StdAfx.h"
#include "AudioCallIncomingDlg.h"

CUcsIncomeingDlg::CUcsIncomeingDlg(void)
{	
	m_pTxtInfo = NULL;
	m_pImageCtrl = NULL;
	m_pNickname = NULL;
}

CUcsIncomeingDlg::~CUcsIncomeingDlg(void)
{

}


UINT CUcsIncomeingDlg::ShowModal()
{
	return CWindowWnd::ShowModal();
}

void CUcsIncomeingDlg::InitWindow()
{
	m_pImageCtrl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("incommingLogo")));
	m_pTxtInfo   =  static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("TxtTips")));	
	m_pNickname =  static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("nickname")));	
}

DuiLib::CDuiString CUcsIncomeingDlg::GetSkinFile()
{
	return _T("Call\\AudioIncoming.xml");
}

void CUcsIncomeingDlg::CreateCallInDlg()
{
	Create(NULL, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);
	ShowWindow(true);

	int cx = GetSystemMetrics(SM_CXFULLSCREEN);
	int cy = GetSystemMetrics(SM_CYFULLSCREEN);

	RECT rc;
	::GetClientRect(*this, &rc);

	// 将窗口设置到右下角
	::SetWindowPos(*this, HWND_TOPMOST, cx-rc.right, cy-rc.bottom, rc.right - rc.left, rc.bottom - rc.top,  SWP_NOSIZE | SWP_NOACTIVATE);
}

void CUcsIncomeingDlg::HandleClickEvent( TNotifyUI& msg )
{
	if ( msg.pSender->GetName() == _T("BnAnswer") )
	{
		nRetCode = CALL_ANSER;
	}
	else if ( msg.pSender->GetName() == _T("BnHangUp") )
	{
		nRetCode = CALL_HANGUP;
	}	
	else if ( msg.pSender->GetName() == _T("btnClose") )
	{
		nRetCode = CLOSE_COMMING_DLG;
	}	
	

	PostMessage(WM_CLOSE, (WPARAM)nRetCode);
}

void CUcsIncomeingDlg::InitalCallInDlg( const tstring strNickname, const bool bVideo)
{
	m_pNickname->SetText(strNickname.c_str());
	if ( bVideo )
	{
		m_pImageCtrl->SetBkImage("Call\\video_incomming.PNG");
		m_pTxtInfo->SetText("发起了视频通话");		
	}
	else
	{
		m_pImageCtrl->SetBkImage("Call\\voice_incomming.PNG");
		m_pTxtInfo->SetText("发起了语音通话");
	}	
}
