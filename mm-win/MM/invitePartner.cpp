#include "invitePartner.h"
#include "HttpClient.h"
#include "json/json.h"
#include "MsgBox.h"
#include "../Common/regexHelper.h"
#include "../Common/Logger.h"
#include "../include/Global_Def.h"
#include "MsgBox.h"
#include "UserManager.h"
#include "Controller.h"
#include "webClient.h"
#include "../Common/regexHelper.h"


#define INVITE_PAGE_VIEW 0
#define MSG_PAGE_VIEW 1

#define MOBILE_NUM_LEN 11

#define  FOCUS_BORDER_COLOR 0xFF76c1db
#define  BORDER_COLOR 0x00CCCCCC


CInvitePartner::CInvitePartner()
{
}

CInvitePartner::~CInvitePartner()
{

}

DuiLib::CDuiString CInvitePartner::GetSkinFile()
{
	return _T("homePage\\invitePartnerUISkin.xml");
}

void CInvitePartner::HandleClickEvent( TNotifyUI& msg )
{
	if (msg.pSender == m_pBtnClearPhoneNo1)
	{
		m_pTxtPhoneNo1->SetText(_T(""));
		m_pBtnClearPhoneNo1->SetVisible(false);
	}
	else if (msg.pSender == m_pBtnClearPhoneNo2)
	{
		m_pTxtPhoneNo2->SetText(_T(""));
		m_pBtnClearPhoneNo2->SetVisible(false);
	}
	else if (msg.pSender == m_pBtnClearPhoneNo3)
	{
		m_pTxtPhoneNo3->SetText(_T(""));
		m_pBtnClearPhoneNo3->SetVisible(false);
	}
	else if (msg.pSender == m_pBtnContinueInvite)
	{
		SetTabView(INVITE_PAGE_VIEW);
	}
	else if (msg.pSender == m_pBtnInvitePartner)
	{
		OnClickInvitePartner();
	}
}


bool CInvitePartner::InvitePartner(const tstring strPhoneNo, CDuiString& strMsg)
{
	if (strPhoneNo.empty())
	{
		return false;
	}

	if (CWebClient::IsValidUser(strPhoneNo))
	{
		if (CController::Instance()->GetUserInfoMgr()->IsMySelf(strPhoneNo))
		{
			strMsg.Format("号码 %s 是您自己", strPhoneNo.c_str());
		}
		else if (CController::Instance()->GetUserInfoMgr()->IsMyFriend(strPhoneNo))
		{
			strMsg.Format("号码 %s 已经是您的好友", strPhoneNo.c_str());
		}
		else 
		{
			CController::Instance()->GetUserInfoMgr()->AddOneUser((tstring)strPhoneNo);
			strMsg.Format("号码 %s 被邀请成功，等待对方回复", strPhoneNo.c_str());
		}
	}
	else
	{
		tstring strErrDes;
		if (CWebClient::InviteBussinessParter(strPhoneNo, strErrDes))
		{
			strMsg.Format("号码 %s 还不是平台用户，已经邀请其加入", strPhoneNo.c_str());
		}
		else
		{
			strMsg.Format("号码 %s 还不是平台用户，邀请其加入平台失败", strPhoneNo.c_str());
		}
	}
}

bool CInvitePartner::ValidatePhoneNo(const tstring& strPhoneNo)
{
	if (!CRegexHelper::IsValidMobileNo(strPhoneNo.c_str()))
	{
		CDuiString strMsg;
		strMsg.Format("手机号码 %s 格式不正确", strPhoneNo.c_str());
		CMsgBox::MsgBox(GetHWND(), "添加商业伙伴", strMsg.GetData(), MSGBOX_ICON_WARNING);
		return false;
	}
	return true;
}

void CInvitePartner::OnClickInvitePartner()
{
	tstring strPhoneNo1 = m_pTxtPhoneNo1->GetText().GetData();
	tstring strPhoneNo2 = m_pTxtPhoneNo2->GetText().GetData();
	tstring strPhoneNo3 = m_pTxtPhoneNo3->GetText().GetData();

	CLabelUI* pLabelPhoneNo1Msg  = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("txtPhone1Msg")));
	CLabelUI* pLabelPhoneNo2Msg  = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("txtPhone2Msg")));
	CLabelUI* pLabelPhoneNo3Msg  = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("txtPhone3Msg")));


	if (strPhoneNo1.empty() && strPhoneNo2.empty() && strPhoneNo3.empty())
	{
		CMsgBox::MsgBox(GetHWND(), "添加商业伙伴", "请至少输入一个手机号码", MSGBOX_ICON_WARNING);
		return;
	}
	else
	{
		if(!strPhoneNo1.empty() && _tcsicmp(strPhoneNo1.c_str(),strPhoneNo2.c_str()) == 0)
		{
			CMsgBox::MsgBox(GetHWND(), "添加商业伙伴", "您输入的手机号码出现重复！", MSGBOX_ICON_WARNING);
			return;
		}
		if(!strPhoneNo1.empty() && _tcsicmp(strPhoneNo1.c_str(),strPhoneNo3.c_str()) == 0)
		{
			CMsgBox::MsgBox(GetHWND(), "添加商业伙伴", "您输入的手机号码出现重复！", MSGBOX_ICON_WARNING);
			return;
		}
		if(!strPhoneNo2.empty() && _tcsicmp(strPhoneNo3.c_str(),strPhoneNo2.c_str()) == 0)
		{
			CMsgBox::MsgBox(GetHWND(), "添加商业伙伴", "您输入的手机号码出现重复！", MSGBOX_ICON_WARNING);
			return;
		}
	}

	
	if (!strPhoneNo1.empty())
	{
		if (!ValidatePhoneNo(strPhoneNo1))
		{
			return;
		}
		CDuiString strMsg;
		InvitePartner(strPhoneNo1, strMsg);
		pLabelPhoneNo1Msg->SetText(strMsg);
		pLabelPhoneNo1Msg->GetParent()->SetVisible(true);
	}
	else 
	{
		pLabelPhoneNo1Msg->GetParent()->SetVisible(false);
	}
	
	if (!strPhoneNo2.empty())
	{
		if (!ValidatePhoneNo(strPhoneNo2))
		{
			return;
		}
		CDuiString strMsg;
		InvitePartner(strPhoneNo2, strMsg);
		pLabelPhoneNo2Msg->SetText(strMsg);
		pLabelPhoneNo2Msg->GetParent()->SetVisible(true);
	}
	else 
	{
		pLabelPhoneNo2Msg->GetParent()->SetVisible(false);
	}

	if (!strPhoneNo3.empty())
	{
		if (!ValidatePhoneNo(strPhoneNo3))
		{
			return;
		}
		CDuiString strMsg;
		InvitePartner(strPhoneNo3, strMsg);
		pLabelPhoneNo3Msg->SetText(strMsg);
		pLabelPhoneNo3Msg->GetParent()->SetVisible(true);
	}
	else 
	{
		pLabelPhoneNo3Msg->GetParent()->SetVisible(false);
	}

	SetTabView(MSG_PAGE_VIEW);
}

void CInvitePartner::SetTabView(int idx)
{
	CTabLayoutUI* pTabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("SwitchLayout")));
	if (pTabLayout)
	{
		pTabLayout->SelectItem(idx);
	}	
	if (INVITE_PAGE_VIEW == idx)
	{
		m_pTxtPhoneNo1->SetText(_T(""));
		m_pTxtPhoneNo2->SetText(_T(""));
		m_pTxtPhoneNo3->SetText(_T(""));
		m_pBtnClearPhoneNo1->SetVisible(false);
		m_pBtnClearPhoneNo2->SetVisible(false);
		m_pBtnClearPhoneNo3->SetVisible(false);
	}
}

void CInvitePartner::InitWindow()
{	
	m_pTxtPhoneNo1 = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("txtPhoneNo1")));
	m_pTxtPhoneNo2 = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("txtPhoneNo2")));
	m_pTxtPhoneNo3 = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("txtPhoneNo3")));

	m_pBtnClearPhoneNo1 = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnClearPhoneNo1")));
	m_pBtnClearPhoneNo2 = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnClearPhoneNo2")));
	m_pBtnClearPhoneNo3 = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnClearPhoneNo3")));

	m_pBtnInvitePartner = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnInvitePartner")));
	m_pBtnContinueInvite = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnContinueInvitePartner")));

	SetTabView(INVITE_PAGE_VIEW);
}

void CInvitePartner::HandleSetFocusEvent(TNotifyUI& msg)
{
	if (msg.pSender == m_pTxtPhoneNo1)
	{
		m_pTxtPhoneNo1->GetParent()->SetBorderColor(FOCUS_BORDER_COLOR);
	}	
	else if (msg.pSender == m_pTxtPhoneNo2)
	{
		m_pTxtPhoneNo2->GetParent()->SetBorderColor(FOCUS_BORDER_COLOR);
	}	
	else if (msg.pSender == m_pTxtPhoneNo3)
	{
		m_pTxtPhoneNo3->GetParent()->SetBorderColor(FOCUS_BORDER_COLOR);
	}	
}

void CInvitePartner::HandleTextChangedEvent( TNotifyUI& msg )
{
	m_pBtnClearPhoneNo1->SetVisible(m_pTxtPhoneNo1->GetText().GetLength() > 0);
	m_pBtnClearPhoneNo2->SetVisible(m_pTxtPhoneNo2->GetText().GetLength() > 0);
	m_pBtnClearPhoneNo3->SetVisible(m_pTxtPhoneNo3->GetText().GetLength() > 0);
}

void CInvitePartner::HandleKillFocusEvent( TNotifyUI& msg )
{
	if (msg.pSender == m_pTxtPhoneNo1)
	{
		m_pTxtPhoneNo1->GetParent()->SetBorderColor(BORDER_COLOR);
	}	
	else if (msg.pSender == m_pTxtPhoneNo2)
	{
		m_pTxtPhoneNo2->GetParent()->SetBorderColor(BORDER_COLOR);
	}	
	else if (msg.pSender == m_pTxtPhoneNo3)
	{
		m_pTxtPhoneNo3->GetParent()->SetBorderColor(BORDER_COLOR);
	}	
}


