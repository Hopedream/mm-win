#include "SearchBussinessPartnerPage.h"

#include "..\Include\Global_Def.h"
#include "SendAddRequestPage.h"
#include "ViewFoundPartnerInfoPage.h"
#include "mainDlg.h"
#include "../Common/regexHelper.h"
#include "MsgBox.h"
#include "../include/Global_Def.h"
#include "HttpClient.h"
#include "json/json.h"
#include "Controller.h"
#include "UserManager.h"
#include "webClient.h"


//qbpp=query_bussiness_partner_page

const TCHAR* btn_minize		= "qbpp_btn_minize";
const TCHAR* btn_close		= "qbpp_btn_close";
const TCHAR* edit_phone		= "qbpp_edit_phone_number";
const TCHAR* btn_query		= "qbpp_btn_query";
const TCHAR* btn_return		= "qbpp_btn_return";
const TCHAR* btn_avatar		= "qbpp_btn_user_avatar";
const TCHAR* label_nick		= "qbpp_label_user_nickname";
const TCHAR* label_company	= "qbpp_label_user_company";
const TCHAR* btn_add_friend = "qbpp_btn_add_friend";


CSearchBussinessPartnerPage::CSearchBussinessPartnerPage()
{
	m_hMainWnd = NULL;
	m_pCurUserInfo = NULL;
	m_pRequestpage = NULL;
	m_pViewPartnerInfoPage = NULL;
	CController::Instance()->GetUserInfoMgr()->AddEventHandler(this);
}

CSearchBussinessPartnerPage::~CSearchBussinessPartnerPage()
{
	if (NULL != m_pRequestpage)
	{
		delete m_pRequestpage;
		m_pRequestpage = NULL;
	}

	if (NULL != m_pCurUserInfo)
	{
		delete m_pCurUserInfo;
		m_pCurUserInfo = NULL;
	}

	if (NULL != m_pViewPartnerInfoPage)
	{
		delete m_pViewPartnerInfoPage;
		m_pViewPartnerInfoPage = NULL;
	}
	CController::Instance()->GetUserInfoMgr()->RemoveEventHandler(this);
}

void CSearchBussinessPartnerPage::HandleClickEvent(TNotifyUI& msg)
{

		CDuiString strControlName = msg.pSender->GetName();

		//关闭界面
		if (_tcsicmp(msg.pSender->GetName(), btn_close) == 0)
		{
			ShowWindow(SW_HIDE);
			m_pBtnQuery->SetEnabled(true);
			//Close();
		}
		else if (_tcsicmp(msg.pSender->GetName(), btn_minize) == 0)
		{
			ShowWindow(SW_HIDE);
			m_pBtnQuery->SetEnabled(true);
			//Close();
		}
		else if (_tcsicmp(msg.pSender->GetName(), btn_return) == 0)
		{
			ShowWindow(SW_HIDE);
		}
		else if (_tcsicmp(msg.pSender->GetName(), btn_add_friend) == 0)
		{
			OnAddPartner();
		}
		else if (_tcsicmp(msg.pSender->GetName(), btn_query) == 0)
		{
			OnQueryPartner();
		}
		else if (_tcsicmp(msg.pSender->GetName(), btn_avatar) == 0)
		{
			OnShowPartnerInfoPage();
		}	
}
//隐藏加好友的按钮
bool CSearchBussinessPartnerPage::Initialize()
{
	CButtonUI* pBtnAdd = static_cast<CButtonUI*>(m_PaintManager.FindControl(btn_add_friend));
	if (NULL != pBtnAdd)
	{
		pBtnAdd->SetVisible(false);
	}

	return true;
}


//初始化窗口
void CSearchBussinessPartnerPage::InitWindow()
{
	m_pBtnQuery = static_cast<CButtonUI*>(m_PaintManager.FindControl(btn_query));	
}

void CSearchBussinessPartnerPage::SetMainWndHWND(HWND hWndMaind)
{
	m_hMainWnd = hWndMaind;
}

void CSearchBussinessPartnerPage::SetSearchResult(sUserVCard& oneCard)
{	
	m_strFindPhoneNo.clear();
	m_pBtnQuery->SetEnabled(true);
	CButtonUI* pBtnAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(btn_avatar));
	if (NULL != pBtnAvatar)
	{	
		bool bExist = PathFileExists(oneCard.strPhotoPath.c_str());
		if (!bExist || oneCard.strPhotoPath.empty())
		{
			pBtnAvatar->SetBkImage(DEF_AVATAR_NOR);
		}
		else
		{
			pBtnAvatar->SetBkImage(oneCard.strPhotoPath.c_str());
		}
	}

	int index = oneCard.strAccount.find('@');
	tstring strPhone = oneCard.strAccount.substr(0,index);

	CLabelUI* pLabelNickname = static_cast<CLabelUI*>(m_PaintManager.FindControl(label_nick));
	if (NULL != pLabelNickname)
	{	
		if (oneCard.strUserNickname.empty())
		{
			pLabelNickname->SetText(strPhone.c_str());
		}
		else
		{
			pLabelNickname->SetText(oneCard.strUserNickname.c_str());
		}
		
	}

	orgInfo org;
	CLabelUI* pLabelCompany = static_cast<CLabelUI*>(m_PaintManager.FindControl(label_company));
	if (CWebClient::GetOrgByPhone(strPhone, org))
	{
		//CLabelUI* pLabelCompany = static_cast<CLabelUI*>(m_PaintManager.FindControl(label_company));
		if (NULL != pLabelCompany)
		{
			pLabelCompany->SetText(org.strName.c_str());
		}
	}
	else
	{
		if (NULL != pLabelCompany)
		{
			pLabelCompany->SetText(org.strName.c_str());
		}
	}


	

	CButtonUI* pAddFriend = static_cast<CButtonUI*>(m_PaintManager.FindControl(btn_add_friend));
	if (NULL != pAddFriend)
	{
		pAddFriend->SetVisible(true);
	}

	//更新cache
	if (NULL != m_pCurUserInfo)
	{
		delete m_pCurUserInfo;
		m_pCurUserInfo = NULL;
	}

	//refresh org info.

	m_pCurUserInfo = new sUserVCard;
	if (NULL != m_pCurUserInfo)
	{
		m_pCurUserInfo->strAccount = oneCard.strAccount;
		m_pCurUserInfo->strUserNickname = oneCard.strUserNickname;
		m_pCurUserInfo->strRemark = oneCard.strRemark;
		m_pCurUserInfo->strGender = oneCard.strGender;	
		m_pCurUserInfo->strSignature = oneCard.strSignature;
		m_pCurUserInfo->strPhone = oneCard.strPhone;
		m_pCurUserInfo->strEmail = oneCard.strEmail;
		m_pCurUserInfo->strAddr = oneCard.strAddr;
		m_pCurUserInfo->strPhotoType = oneCard.strPhotoType;
		m_pCurUserInfo->strPhotoBinary = oneCard.strPhotoBinary;
		m_pCurUserInfo->strUrl = oneCard.strUrl;
		m_pCurUserInfo->strPhotoPath = oneCard.strPhotoPath;
		m_pCurUserInfo->strGrayAvatar = oneCard.strGrayAvatar;
		m_pCurUserInfo->strAge = oneCard.strAge;
		m_pCurUserInfo->strOrganization = org.strName;
	}
}

void CSearchBussinessPartnerPage::ClearSearchResult()
{
	CButtonUI* pBtnAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(btn_avatar));
	if (NULL != pBtnAvatar)
	{	
		pBtnAvatar->SetBkImage("");
	}

	CLabelUI* pLabelNickname = static_cast<CLabelUI*>(m_PaintManager.FindControl(label_nick));
	if (NULL != pLabelNickname)
	{
		pLabelNickname->SetText("");
	}

	CLabelUI* pLabelCompany = static_cast<CLabelUI*>(m_PaintManager.FindControl(label_company));
	if (NULL != pLabelCompany)
	{
		pLabelCompany->SetText("");
	}

	CButtonUI* pAddFriend = static_cast<CButtonUI*>(m_PaintManager.FindControl(btn_add_friend));
	if (NULL != pAddFriend)
	{
		pAddFriend->SetVisible(false);
	}

	if (NULL != m_pCurUserInfo)
	{
		delete m_pCurUserInfo;
		m_pCurUserInfo = NULL;
	}
}


void CSearchBussinessPartnerPage::OnQueryPartner()
{
	m_pBtnQuery->SetEnabled(false);
	if (!m_strFindPhoneNo.empty())
	{
		CMsgBox* pMsgBox = new CMsgBox("查找", "正在查找......",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return;
	}
	

	CEditUI* pUsername = static_cast<CEditUI*>(m_PaintManager.FindControl(edit_phone));
	m_strFindPhoneNo = pUsername->GetText().GetData();
	if (m_strFindPhoneNo.empty())
	{
		CMsgBox* pMsgBox = new CMsgBox("查找", "请输入手机号",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		m_pBtnQuery->SetEnabled(true);
		return;
	}
	
	if (!CRegexHelper::IsValidMobileNo(m_strFindPhoneNo.c_str()))
	{
		CMsgBox* pMsgBox = new CMsgBox("查找", "输入的手机号码格式错误",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		m_strFindPhoneNo.clear();
		m_pBtnQuery->SetEnabled(true);
		return;
	}
	
	if (CWebClient::IsValidUser(m_strFindPhoneNo))
	{
		CController::Instance()->GetUserInfoMgr()->FetchVCardByAccount(m_strFindPhoneNo);		
	}
	else
	{
		ClearSearchResult();			
		CMsgBox* pMsgBox = new CMsgBox("查找联系人", "该用户还不是平台用户，是否需要邀请其加入平台？", MSGBOX_ICON_ASK, MSGBOX_IDYES|MSGBOX_IDNO);
		if (MSGBOX_IDYES == pMsgBox->ShowModal(this->GetHWND()))
		{
			tstring strErrDes;
			if (CWebClient::InviteBussinessParter(m_strFindPhoneNo, strErrDes))
			{
					CMsgBox* pMsgBox = new CMsgBox("添加商业伙伴", "添加商业伙伴成功");
					pMsgBox->ShowModal();
			}		
		}
		m_strFindPhoneNo.clear();
		m_pBtnQuery->SetEnabled(true);
	}	
}

void CSearchBussinessPartnerPage::OnAddPartner()
{
	if (CController::Instance()->GetUserInfoMgr()->IsMySelf(m_pCurUserInfo->strAccount))
	{
		CMsgBox* pMsgBox = new CMsgBox("添加商业伙伴", "该联系人是您自己", MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return;
	}

	if (CController::Instance()->GetUserInfoMgr()->IsMyFriend(m_pCurUserInfo->strAccount))
	{
		CMsgBox* pMsgBox = new CMsgBox("添加商业伙伴", "该联系人已经是您的好友", MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return;
	}

	if (NULL == m_pRequestpage)
	{
		m_pRequestpage = new CSendAddRequestPage;	
		if (NULL != m_pRequestpage)
		{
			m_pRequestpage->SetMainWndHWND(m_hMainWnd);
			m_pRequestpage->CreateWnd();
			m_pRequestpage->Initialize(m_pCurUserInfo);
		}
	}
	else
	{
		m_pRequestpage->Initialize(m_pCurUserInfo);
		m_pRequestpage->ShowWindow(true);
	}
}

void CSearchBussinessPartnerPage::OnShowPartnerInfoPage()
{
	if (NULL == m_pViewPartnerInfoPage)
	{
		m_pViewPartnerInfoPage = new CViewFoundPartnerInfoPage(this);
		if (NULL != m_pViewPartnerInfoPage)
		{
			m_pViewPartnerInfoPage->CreateWnd();
			m_pViewPartnerInfoPage->Initialize(m_pCurUserInfo);
		}
	}
	else
	{
		m_pViewPartnerInfoPage->Initialize(m_pCurUserInfo);
		m_pViewPartnerInfoPage->ShowWindow(true);
	}
}


void CSearchBussinessPartnerPage::OnMySelfCardInfo( sUserVCard& oneInfo )
{	
	if  (tstring::npos != oneInfo.strAccount.find(m_strFindPhoneNo))
	{		
		SetSearchResult(oneInfo);
	}
}

void CSearchBussinessPartnerPage::OnStrangerVCard( tstring& strAccount, sUserVCard& oneInfo )
{
	if  (tstring::npos != strAccount.find(m_strFindPhoneNo))
	{
		SetSearchResult(oneInfo);
	}
}


void CSearchBussinessPartnerPage::OnVCardReturned( tstring& strAccount, sUserVCard& oneInfo )
{
	if  (tstring::npos != strAccount.find(m_strFindPhoneNo))
	{
		SetSearchResult(oneInfo);
	}
}


