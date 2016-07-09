#include "volumeUI.h"
#include "UCSNetServiceCallBackInterface.h"
CVolumeUI::CVolumeUI() 
{
	m_hParentWnd = NULL;
	m_pSliderUI = NULL;
}

CVolumeUI::~CVolumeUI()
{

}

void CVolumeUI::Init( HWND hWndParent, POINT ptPos )
{
	m_hParentWnd = hWndParent;
	ptPos.x = 64;
	ptPos.y = 450;
	Create(hWndParent, _T("MenuWnd"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	::ClientToScreen(hWndParent, &ptPos);
	::SetWindowPos(m_hWnd, NULL, ptPos.x, ptPos.y - 120, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);

	m_pSliderUI = static_cast<CSliderUI*>(m_PaintManager.FindControl(_T("volumeSlider"))); 
	if (NULL != m_pSliderUI)
	{
		m_pSliderUI->SetValue(CUcPaasClient::Instance()->GetVolumeValue());
	}
}


LRESULT CVolumeUI::HandleMessage( UINT uMsg, WPARAM wParam, LPARAM lParam )
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

LRESULT CVolumeUI::OnKillFocus( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	Close();
	bHandled = FALSE;
	return __super::OnKillFocus(uMsg, wParam, lParam, bHandled); 
}



DuiLib::CDuiString CVolumeUI::GetSkinFile()
{
	return _T("Call\\volumeMenuUI.xml");
}

void CVolumeUI::HandleValueChangedEvent( TNotifyUI& msg )
{	
	if (msg.pSender == m_pSliderUI)
	{
		CUcPaasClient::Instance()->SetVolumeValue(m_pSliderUI->GetValue());
	}
}
