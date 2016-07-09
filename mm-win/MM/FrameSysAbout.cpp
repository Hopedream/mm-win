#include "FrameSysAbout.h"
#include "mainDlg.h"
#include "webClient.h"

CFrameSysAbout* CFrameSysAbout::m_pInstance = NULL;
CFrameSysAbout::CFrameSysAbout()
{
}


CFrameSysAbout::~CFrameSysAbout(void)
{
}

CFrameSysAbout* CFrameSysAbout::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CFrameSysAbout;
		m_pInstance->CreateWnd(mainDlg::getInstance()->GetHWND());
		//m_pInstance->SetTopWnd();
	}
	return m_pInstance;
}

void CFrameSysAbout::HandleSetFocusEvent( TNotifyUI& msg )
{

}

void CFrameSysAbout::HandleKillFocusEvent( TNotifyUI& msg )
{

}

void CFrameSysAbout::HandleTextChangedEvent( TNotifyUI& msg )
{

}

void CFrameSysAbout::HandleClickEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("buttonClose")) == 0)
	{
		ShowWindow(false);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonOK")) == 0)
	{
		ShowWindow(false);
	}
}

DuiLib::CDuiString CFrameSysAbout::GetSkinFile()
{
	return _T("sys_about.xml");
}

CControlUI* CFrameSysAbout::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CFrameSysAbout::InitWindow()
{
	//CWebClient::getRegions();
	tstring strtmp = CWebClient::getRegions();
	int i=0;
}

LRESULT CFrameSysAbout::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return 0;
}
