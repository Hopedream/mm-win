#include "FrameGuideUser.h"
#include "mainDlg.h"
#include "MsgBox.h"
#include "Controller.h"
#include "FrameGuideOrg.h"

CFrameGuideUser* CFrameGuideUser::m_pInstance = NULL;
CFrameGuideUser::CFrameGuideUser()
{
	CController::Instance()->GetUserInfoMgr()->AddEventHandler((IUserManagerEvent*)this);
	m_bContinue = false;
}


CFrameGuideUser::~CFrameGuideUser(void)
{
	CController::Instance()->GetUserInfoMgr()->RemoveEventHandler((IUserManagerEvent*)this);
}

void CFrameGuideUser::HandleSetFocusEvent( TNotifyUI& msg )
{

}

void CFrameGuideUser::HandleKillFocusEvent( TNotifyUI& msg )
{

}

void CFrameGuideUser::HandleTextChangedEvent( TNotifyUI& msg )
{

}

void CFrameGuideUser::HandleClickEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("btn_close")) == 0)
	{
		this->ShowWindow(false);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("btn_cancel")) == 0)
	{
		m_bContinue = true;
		SavePersonInfo();
		
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("btn_ok")) == 0)
	{
		m_bContinue = false;
		SavePersonInfo();
		
	}
}

void CFrameGuideUser::HandleItemSelectEvent( TNotifyUI& msg )
{

}

DuiLib::CDuiString CFrameGuideUser::GetSkinFile()
{
	return _T("guide_user.xml");
}

CControlUI* CFrameGuideUser::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CFrameGuideUser::InitWindow()
{
	m_pEdtName = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("edt_user_name")));
	m_pCombVocation = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("cmb_vocation")));
}

CFrameGuideUser* CFrameGuideUser::Instance()
{

	if(NULL == m_pInstance)
	{
		m_pInstance = new CFrameGuideUser;
		m_pInstance->CreateWnd();
	}
	return m_pInstance;
}

void CFrameGuideUser::SavePersonInfo()
{
	if (m_pEdtName->GetText().GetLength() < 1)
	{
		CMsgBox* pMsgBox = new CMsgBox("提示", "用户名称不能为空！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return;
	}
	else
	{
		m_myUserInfo.strUserNickname = m_pEdtName->GetText();
	}
	if (m_pCombVocation->GetText().GetLength() < 1)
	{
		CMsgBox* pMsgBox = new CMsgBox("提示", "请选择所属行业！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return;
	}
	else
	{
		m_myUserInfo.strVocation = m_pCombVocation->GetText();
	}
	CController::Instance()->GetUserInfoMgr()->StoreMyVCard(m_myUserInfo);
}

void CFrameGuideUser::updateinfo()
{
	//初始化ui
	m_pEdtName->SetText(m_myUserInfo.strUserNickname.c_str());
}

void CFrameGuideUser::OnMySelfCardInfo( sUserVCard& oneInfo )
{
	m_myUserInfo = oneInfo;
	updateinfo();
}

void CFrameGuideUser::OnStoreVCardResult( tstring& strAccount, bool bSucc )
{
	if (bSucc)
	{
		CController::Instance()->GetUserInfoMgr()->FetchVCardByAccount(m_myUserInfo.strAccount);
		ShowWindow(false);
		if (!m_bContinue)
		{
			CMsgBox* pMsgBox = new CMsgBox("提示", "修改用户信息成功！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
		}
		else
		{
			CFrameGuideOrg::Instance()->ShowWindow(true);
			//mainDlg::getInstance()->showGuideOrg();
		}
		
	}
	else 
	{
		CMsgBox* pMsgBox = new CMsgBox("提示", "修改用户信息失败！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
	}
}

bool CFrameGuideUser::getContinue()
{
	return m_bContinue;
}
