#include "FrameSysQuit.h"
#include "mainDlg.h"

CFrameSysQuit* CFrameSysQuit::m_pInstance = NULL;
CFrameSysQuit::CFrameSysQuit()
{
}


CFrameSysQuit::~CFrameSysQuit(void)
{
}

CFrameSysQuit* CFrameSysQuit::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CFrameSysQuit;
		m_pInstance->CreateWnd(mainDlg::getInstance()->GetHWND());
		//m_pInstance->SetTopWnd();
	}
	return m_pInstance;
}

void CFrameSysQuit::HandleSetFocusEvent( TNotifyUI& msg )
{

}

void CFrameSysQuit::HandleKillFocusEvent( TNotifyUI& msg )
{

}

void CFrameSysQuit::HandleTextChangedEvent( TNotifyUI& msg )
{

}

void CFrameSysQuit::HandleClickEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("button_close")) == 0)
	{
		ShowWindow(false);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("button_ok")) == 0)
	{
		mainDlg::getInstance()->OnExit(msg);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("button_cancel")) == 0)
	{
		ShowWindow(false);
	}
}

DuiLib::CDuiString CFrameSysQuit::GetSkinFile()
{
	return _T("sys_quit.xml");
}

CControlUI* CFrameSysQuit::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CFrameSysQuit::InitWindow()
{

}

LRESULT CFrameSysQuit::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return 0;
}
