#include "FrameSysQuitOrg.h"
#include "Controller.h"
#include "mainDlg.h"
#include "MsgBox.h"

CFrameSysQuitOrg* CFrameSysQuitOrg::m_pInstance = NULL;
CFrameSysQuitOrg::CFrameSysQuitOrg()
{
}


CFrameSysQuitOrg::~CFrameSysQuitOrg(void)
{
}

CFrameSysQuitOrg* CFrameSysQuitOrg::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CFrameSysQuitOrg;
		m_pInstance->CreateWnd(mainDlg::getInstance()->GetHWND());
		//m_pInstance->SetTopWnd();
	}
	return m_pInstance;
}

void CFrameSysQuitOrg::HandleSetFocusEvent( TNotifyUI& msg )
{

}

void CFrameSysQuitOrg::HandleKillFocusEvent( TNotifyUI& msg )
{

}

void CFrameSysQuitOrg::HandleTextChangedEvent( TNotifyUI& msg )
{

}

void CFrameSysQuitOrg::HandleClickEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("button_close")) == 0)
	{
		ShowWindow(false);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("button_ok")) == 0)
	{
		quitOrg();
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("button_cancel")) == 0)
	{
		ShowWindow(false);
	}
}

DuiLib::CDuiString CFrameSysQuitOrg::GetSkinFile()
{
	return _T("sys_quit_org.xml");
}

CControlUI* CFrameSysQuitOrg::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CFrameSysQuitOrg::InitWindow()
{

}

LRESULT CFrameSysQuitOrg::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	return 0;
}

void CFrameSysQuitOrg::quitOrg()
{
	getOrgInfo();   //todo
	m_strPhone = mainDlg::getInstance()->getMyUserInfo().strPhone;
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/sign-out-org");
	//tstring strPost = _T("");
	//strPost += _T("oid=");
	//tstring strNameDefault = m_strOid;
	//strPost += strNameDefault;
	//strPost += _T("&cell_phone=") + m_strPhone; // + "]";           //110000,110100,110101");
	//strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	int iResIndex = strHttpResponse.find("\"result\":1");
	if (strHttpResponse.find("\"result\":1") < 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("退出机构", "退出机构失败！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return;
	}
	else
	{
		//删除成功
		CMsgBox* pMsgBox = new CMsgBox("退出机构", "退出机构成功！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		ShowWindow(false);
		mainDlg::getInstance()->PostMessage(WM_EXIT_ORG);
		return;
	}	
}

void CFrameSysQuitOrg::getOrgInfo()
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-org");
	tstring strPost; // = _T("oid=");

	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Post(strUrl, strPost, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") == -1)
	{
		m_strOid = strHttpResponse.substr(7, strHttpResponse.find("\",\"") - 7);
	}
	else
	{
		m_strOid = "-1";
	}
}
