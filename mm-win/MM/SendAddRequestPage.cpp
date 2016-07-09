#include "SendAddRequestPage.h"

#include "Utility.h"
#include "MsgBox.h"
#include <sstream>
#include "HttpClient.h"
#include "..\Include\Global_Def.h"
#include "Controller.h"
#include "UserManager.h"
#include "MsgBox.h"
#include "StringHelper.h"

using namespace std;

const TCHAR* sarp_btn_avatar		= "sarp_button_avatar";
const TCHAR* sarp_label_nick		= "sarp_label_nick";
const TCHAR* sarp_label_company		= "sarp_label_company";
const TCHAR* sarp_label_hangye		= "sarp_label_hangye";
const TCHAR* sarp_label_phone		= "sarp_label_phone";
const TCHAR* sarp_btn_minize		= "sarp_btn_minize";
const TCHAR* sarp_btn_close			= "sarp_btn_close";
const TCHAR* sarp_edit_tips			= "sarp_edit_tips";
const TCHAR* sarp_btn_send			= "sarp_button_send";
const TCHAR* sarp_btn_close_2		= "sarp_button_close";
const TCHAR* sarp_richedit_tips		= "RicheditaddPartnerTips";

#define AUTH_INFO "我是..."
#define AUTH_INFO_SETFOCUS "我是"

CSendAddRequestPage::CSendAddRequestPage()
{
	m_hMainWnd = NULL;
}

CSendAddRequestPage::~CSendAddRequestPage()
{

}

void CSendAddRequestPage::HandleClickEvent(TNotifyUI& msg)
{

		CDuiString strControlName = msg.pSender->GetName();

		//关闭界面
		if (_tcsicmp(msg.pSender->GetName(), sarp_btn_close) == 0)
		{
			ShowWindow(SW_HIDE);
			//Close();
		}
		if (_tcsicmp(msg.pSender->GetName(), sarp_btn_close_2) == 0)
		{
			ShowWindow(SW_HIDE);
		}
		else if (_tcsicmp(msg.pSender->GetName(), sarp_btn_minize) == 0)
		{
			ShowWindow(SW_HIDE);
			//Close();
		}
		else if (_tcsicmp(msg.pSender->GetName(), sarp_btn_send) == 0)
		{
			CEditUI* pRichEditUI = static_cast<CEditUI*>(m_PaintManager.FindControl(sarp_richedit_tips));
			tstring strMsg;
			if (NULL != pRichEditUI)
			{
				strMsg = pRichEditUI->GetText().GetData();
				if (0 == strMsg.compare(AUTH_INFO) || 0 == strMsg.compare(AUTH_INFO_SETFOCUS))
				{
					strMsg.clear();
				}
			}
			CController::Instance()->GetUserInfoMgr()->AddOneUser(m_strCurAccount, CChineseCode::EncodeUTF8(strMsg));
			CMsgBox* pMsgBox = new CMsgBox("添加商业伙伴", "您的商业伙伴添加请求已发送，正在等待对方确认！");
			pMsgBox->ShowModal(this->GetHWND());
			ShowWindow(SW_HIDE);
		}	
}

//获取用户信息，并初始化到界面
bool CSendAddRequestPage::Initialize(sUserVCard* pUserInfo)
{
	if (NULL == pUserInfo)
	{
		return false;
	}

	m_strCurAccount = pUserInfo->strAccount;

	CButtonUI* pBtnAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(sarp_btn_avatar));
	if (NULL != pBtnAvatar)
	{	
		bool bExist = PathFileExists(pUserInfo->strPhotoPath.c_str());
		if (!bExist || pUserInfo->strPhotoPath.empty())
		{
			pBtnAvatar->SetBkImage(DEF_AVATAR_NOR);
		}
		else
		{
			pBtnAvatar->SetBkImage(pUserInfo->strPhotoPath.c_str());
		}
	}

	int index = pUserInfo->strAccount.find('@');
	tstring strPhone = pUserInfo->strAccount.substr(0,index);


	CLabelUI* pLabelNickname = static_cast<CLabelUI*>(m_PaintManager.FindControl(sarp_label_nick));
	if (NULL != pLabelNickname)
	{
		if (pUserInfo->strUserNickname.empty())
		{
			pLabelNickname->SetText(strPhone.c_str());
		}
		else
		{
			pLabelNickname->SetText(pUserInfo->strUserNickname.c_str());
		}
		
	}

	CLabelUI* pLabelCompany = static_cast<CLabelUI*>(m_PaintManager.FindControl(sarp_label_company));
	if (NULL != pLabelCompany)
	{
		pLabelCompany->SetText(pUserInfo->strOrganization.c_str());
	}

	CLabelUI* pHangYe = static_cast<CLabelUI*>(m_PaintManager.FindControl(sarp_label_hangye));
	if (NULL != pHangYe)
	{
		pHangYe->SetText("汽车配件");
	}

	CLabelUI* pPhone= static_cast<CLabelUI*>(m_PaintManager.FindControl(sarp_label_phone));
	if (NULL != pPhone)
	{
		pPhone->SetText(strPhone.c_str());
	}

	CEditUI* pRichEditTips = static_cast<CEditUI*>(m_PaintManager.FindControl(sarp_richedit_tips));
	if (NULL != pRichEditTips)
	{
		pRichEditTips->SetText(AUTH_INFO);
		pRichEditTips->SetTextColor(GRAY_TEXT_COLOR);
	}

	return true;
}

//void CSendAddRequestPage::CreateWnd()
//{
//	Create(NULL, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);		
//	CenterWindow();	
//}



void CSendAddRequestPage::HandleSetFocusEvent( TNotifyUI& msg )
{
	CEditUI* pRichEditTips = static_cast<CEditUI*>(m_PaintManager.FindControl(sarp_richedit_tips));			
	if (pRichEditTips == msg.pSender)
	{
		pRichEditTips->SetText(AUTH_INFO_SETFOCUS);
		pRichEditTips->SetTextColor(BLACK_TEXT_COLOR);		
	}		
}
