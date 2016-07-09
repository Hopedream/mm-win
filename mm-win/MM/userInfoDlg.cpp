#include "userInfoDlg.h"

//const TCHAR* const kLoginButtonControlName = _T("buttonLogin");
//const TCHAR* const kCancelButtonControlName = _T("buttonCancel");
//const TCHAR* const kUserEditControlName = _T("editUser");
//const TCHAR* const kpasswordEditControlName = _T("editPassword");

const TCHAR* const kbuttonMinControlName = _T("buttonMin");
const TCHAR* const kbuttonMaxControlName = _T("buttonMax");
const TCHAR* const kbuttonCloseControlName = _T("buttonClose");
const TCHAR* const kbuttonCancelControlName = _T("buttonCancel");
const TCHAR* const kbuttonOKControlName = _T("buttonOK");
const TCHAR* const keditNicknameControlName = _T("nickNameEdit");
const TCHAR* const keditSexControlName = _T("sexEdit");
const TCHAR* const keditMobileControlName = _T("mobileEdit");
const TCHAR* const keditCompanyControlName = _T("companyEdit");
const TCHAR* const keditEmailControlName = _T("emailEdit");


userInfoDlg::userInfoDlg()
{

	
}

userInfoDlg::~userInfoDlg()
{
	

}

CDuiString userInfoDlg::GetSkinFile()
{
	return _T("userInfoDlg.xml");
}

CDuiString userInfoDlg::GetSkinFolder()
{

	return  _T("res\\");
}

LRESULT userInfoDlg::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
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

void userInfoDlg::OnFinalMessage(HWND hWnd)
{
	WindowImplBase::OnFinalMessage(hWnd);
	delete this;
}

void userInfoDlg::OnExit(TNotifyUI& msg)
{
	Close();
}

void userInfoDlg::InitWindow()
{}

LRESULT userInfoDlg::ResponseDefaultKeyEvent(WPARAM wParam, LPARAM lPara, bool& bHandled)
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

UILIB_RESOURCETYPE userInfoDlg::GetResourceType() const
{
	return UILIB_ZIPRESOURCE;
}

CControlUI* userInfoDlg::CreateControl(LPCTSTR pstrClass)
{

	return NULL;
}

LRESULT userInfoDlg::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return 0;
}


LRESULT userInfoDlg::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 0;
}

LPCTSTR userInfoDlg::GetResourceID() const
{
	return MAKEINTRESOURCE(101);
}

LPCTSTR userInfoDlg::GetWindowClassName() const
{
	return _T("MMGui");
}

void userInfoDlg::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		if (_tcsicmp(msg.pSender->GetName(), kbuttonCloseControlName) == 0)
		{
			//todo
			::ShowWindow(*this, SW_HIDE);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kbuttonMaxControlName) == 0)
		{
			//todo
		}
		else if (_tcsicmp(msg.pSender->GetName(), kbuttonMinControlName) == 0)
		{
			//todo
			::ShowWindow(*this, SW_HIDE);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kbuttonCancelControlName) == 0)
		{
			//todo
			::ShowWindow(*this, SW_HIDE);
		}
		else if (_tcsicmp(msg.pSender->GetName(), kbuttonOKControlName) == 0)
		{
			//todo
			CEditUI* pnicknameEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(keditNicknameControlName));
			CDuiString strnickname = pnicknameEdit->GetText();

			CEditUI* psexEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(keditSexControlName));
			CDuiString strsex = psexEdit->GetText();

			CEditUI* pcompanyEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(keditCompanyControlName));
			CDuiString strcompany = pcompanyEdit->GetText();

			CEditUI* pmobileEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(keditMobileControlName));
			CDuiString strmobile = pmobileEdit->GetText();

			CEditUI* pemailEdit = static_cast<CEditUI*>(m_PaintManager.FindControl(keditEmailControlName));
			CDuiString stremail = pemailEdit->GetText();

			::ShowWindow(*this, SW_HIDE);
		}
	}
}





