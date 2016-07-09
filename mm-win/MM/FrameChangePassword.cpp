#include "FrameChangePassword.h"
#include "mainDlg.h"
#include "../Common/regexHelper.h"
#include "Controller.h"

#define TIMERID_VERIFY_CODE  1
#define GET_PINCODE_TIPTEXT _T("������֤��")

CFrameChangePassword* CFrameChangePassword::m_pInstance = NULL;

CFrameChangePassword::CFrameChangePassword()
{
}


CFrameChangePassword::~CFrameChangePassword(void)
{
}

CFrameChangePassword* CFrameChangePassword::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CFrameChangePassword;
		m_pInstance->CreateWnd(mainDlg::getInstance()->GetHWND());
		//m_pInstance->SetTopWnd();
	}
	return m_pInstance;
}

void CFrameChangePassword::HandleSetFocusEvent( TNotifyUI& msg )
{

}

void CFrameChangePassword::HandleKillFocusEvent( TNotifyUI& msg )
{

}

void CFrameChangePassword::HandleTextChangedEvent( TNotifyUI& msg )
{

}

void CFrameChangePassword::HandleClickEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("btnGetPinCode")) == 0)
	{
		GetVerifyCode();
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonClose")) == 0)
	{
		clearDlg();
		ShowWindow(false);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonOK")) == 0)
	{
		changePasswd();	
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonCancel")) == 0)
	{
		clearDlg();
		ShowWindow(false);
	}
	
}

DuiLib::CDuiString CFrameChangePassword::GetSkinFile()
{
	return _T("sys_change_password.xml");
}

CControlUI* CFrameChangePassword::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CFrameChangePassword::InitWindow()
{
	m_pLableTip = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("label_tip")));
	m_pBtnGetPinCode = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnGetPinCode")));
	m_pPasswdEditUI = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editPasswd")));
	m_pConfirmPasswdEditUI = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editConfirmPasswd")));
	m_pPinCodeEditUI = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("editPinCode")));
	m_pPasswdEditUI->SetPasswordMode(true);
	m_pConfirmPasswdEditUI->SetPasswordMode(true);
}

LRESULT CFrameChangePassword::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (WM_TIMER == uMsg && TIMERID_VERIFY_CODE == wParam)
	{
		int nLeftTime = --m_nPinLifecycle;
		if (nLeftTime > 0)
		{
			TCHAR szMsg[100];			
			_stprintf(szMsg,_T("ʣ��%d��"), nLeftTime);
			m_pBtnGetPinCode->SetText(szMsg);
		}
		else
		{
			KillTimer(m_PaintManager.GetPaintWindow(), TIMERID_VERIFY_CODE);
			m_pBtnGetPinCode->SetText(GET_PINCODE_TIPTEXT);
			m_pBtnGetPinCode->SetEnabled(true);
			//m_pBtnGetPinCode->SetNormalImage(IMG_GET_PIN_ACTIVE_BKIMG);
		}
	}
	//bHandled = TRUE;
	return 0;
}

void CFrameChangePassword::GetVerifyCode()
{
	tstring strPhone = mainDlg::getInstance()->getMyselfItem().m_strAccount; //�ֻ�����
	strPhone = strPhone.substr(0, strPhone.find("@"));
	m_strPhone = strPhone;
	if (!CRegexHelper::IsValidMobileNo(strPhone.c_str()))
	{
		m_pLableTip->SetText(_T("�ֻ��������"));
		return ;
	}
	
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("site/create-verify-code?tid=2");

	tstring strPost = (_T(""));
	strPost += _T("cell_phone=");
	strPost += strPhone;

	CHttpClient httpClient;
	tstring strHttpResponse;
	int retCode = httpClient.Post(strUrl, strPost,strHttpResponse);
	if (strHttpResponse.find("\"result\":1") == -1)
	{
		m_pLableTip->SetText(_T("��ȡ��֤��ʧ��"));
		return;
	}
	else
	{
		m_pLableTip->SetText(_T("ϵͳ���������ֻ�������֤�룬��ע����գ�"));
	}

	SetTimer(m_PaintManager.GetPaintWindow(), TIMERID_VERIFY_CODE, 1000,NULL);
	m_pBtnGetPinCode->SetText("ʣ��120��");
	m_pBtnGetPinCode->SetEnabled(false);
	//m_pBtnGetPinCode->SetNormalImage(IMG_GET_PIN_BKIMG);
	m_nPinLifecycle = 120;//root["verify"]["lifecycle"].asInt();	
}

void CFrameChangePassword::changePasswd()
{
	tstring strPasswd = m_pPasswdEditUI->GetText().GetData();
	tstring strConfirmPasswd = m_pConfirmPasswdEditUI->GetText().GetData();
	tstring strverifycode = m_pPinCodeEditUI->GetText().GetData();
	if (strPasswd.length() <= 0 || strConfirmPasswd.length()<= 0)
	{
		m_pLableTip->SetText(_T("���벻��Ϊ��"));

		KillTimer(m_PaintManager.GetPaintWindow(), TIMERID_VERIFY_CODE);
		m_pBtnGetPinCode->SetText(GET_PINCODE_TIPTEXT);
		m_pBtnGetPinCode->SetEnabled(true);
		return;
	}
	if (strverifycode.length() <=0)
	{
		m_pLableTip->SetText(_T("��������֤��"));

		KillTimer(m_PaintManager.GetPaintWindow(), TIMERID_VERIFY_CODE);
		m_pBtnGetPinCode->SetText(GET_PINCODE_TIPTEXT);
		m_pBtnGetPinCode->SetEnabled(true);
		return;
	}

	if (0 != strPasswd.compare(strConfirmPasswd))
	{
		m_pLableTip->SetText(_T("������������벻һ��"));

		KillTimer(m_PaintManager.GetPaintWindow(), TIMERID_VERIFY_CODE);
		m_pBtnGetPinCode->SetText(GET_PINCODE_TIPTEXT);
		m_pBtnGetPinCode->SetEnabled(true);
		return;
	}
	else
	{
		//�޸�����api
		//1.�޸�����
		tstring strUrl(WEB_SERVER_BASE_URL);
		strUrl += _T("user/modify-pwd");

		tstring strPost = (_T(""));
		strPost += _T("cell_phone=");
		strPost += m_strPhone;
		strPost += _T("&new_pwd=");
		strPost += strPasswd;
		strPost += _T("&verify_code=");
		strPost += strverifycode;

		CHttpClient httpClient;
		tstring strHttpResponse;
		tstring strToken = CController::Instance()->GetToken();
		int retCode = httpClient.Post(strUrl, strPost,	strToken, strHttpResponse);

		if (strHttpResponse.find("\"result\":1") == -1)
		{
			m_pLableTip->SetText(_T("�޸�����ʧ�ܣ�"));
		}
		else
		{
			clearDlg();
			m_pLableTip->SetText(_T("��ϲ�޸�����ɹ���"));	
		}
		//3. ��ϲ�޸�����ɹ�
	}
}

void CFrameChangePassword::clearDlg()
{
	KillTimer(m_PaintManager.GetPaintWindow(), TIMERID_VERIFY_CODE);
	m_pBtnGetPinCode->SetText(GET_PINCODE_TIPTEXT);
	m_pBtnGetPinCode->SetEnabled(true);
	m_pConfirmPasswdEditUI->SetText(_T(""));
	m_pPasswdEditUI->SetText(_T(""));
	m_pPinCodeEditUI->SetText(_T(""));
	m_pLableTip->SetText(_T(""));
}

void CFrameChangePassword::showFocus()
{
	static_cast<CButtonUI*>(m_PaintManager.FindControl("buttonClose"))->SetFocus();
}
