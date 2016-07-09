#include "ViewFoundPartnerInfoPage.h"

#include <sstream>
#include "..\Include\Global_Def.h"
#include "SearchBussinessPartnerPage.h"

using namespace std;

const TCHAR* vbp_btn_avatar			= _T("vbp_LabelBussinessPartnerAvatar");
const TCHAR* vbp_label_nick			= _T("vbp_parnter_nick");
const TCHAR* vbp_label_company		= _T("vbp_label_partner_company");
const TCHAR* vbp_label_hangye		= _T("vbp_label_hangye");
const TCHAR* vbp_label_phone		= _T("vbp_label_phonenumber");
const TCHAR* vbp_company_phone		= _T("vbp_label_comp_phone");
const TCHAR* vbp_address			= _T("vbp_label_address");
const TCHAR* vbp_add_partner		= _T("vbp_btn_add_partner");
const TCHAR* vbp_btn_min			= _T("vbp_btn_minize");
const TCHAR* vbp_btn_close			= _T("vbp_btn_close");


CViewFoundPartnerInfoPage::CViewFoundPartnerInfoPage(WindowImplBase* pParent/*=NULL*/)
{
	m_pParentWnd = pParent;
}

CViewFoundPartnerInfoPage::~CViewFoundPartnerInfoPage()
{

}

void CViewFoundPartnerInfoPage::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		CDuiString strControlName = msg.pSender->GetName();

		//关闭界面
		if (_tcsicmp(msg.pSender->GetName(), vbp_btn_close) == 0)
		{
			ShowWindow(SW_HIDE);
			//Close();
		}
		else if (_tcsicmp(msg.pSender->GetName(), vbp_btn_min) == 0)
		{
			ShowWindow(SW_HIDE);
			//Close();
		}
		else if (_tcsicmp(msg.pSender->GetName(), vbp_add_partner) == 0)
		{
			if (NULL != m_pParentWnd)
			{
				((CSearchBussinessPartnerPage*)m_pParentWnd)->OnAddPartner();
			}
		}
	}
}
//获取用户信息，并初始化到界面
bool CViewFoundPartnerInfoPage::Initialize(sUserVCard* pUserInfo)
{
	if (NULL == pUserInfo)
	{
		return false;
	}

	m_strCurAccount = pUserInfo->strAccount;

	CButtonUI* pBtnAvatar = static_cast<CButtonUI*>(m_PaintManager.FindControl(vbp_btn_avatar));
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


	CLabelUI* pLabelNickname = static_cast<CLabelUI*>(m_PaintManager.FindControl(vbp_label_nick));
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

	CLabelUI* pLabelCompany = static_cast<CLabelUI*>(m_PaintManager.FindControl(vbp_label_company));
	if (NULL != pLabelCompany)
	{
		tstring strOrgName = pUserInfo->strOrganization;
		pLabelCompany->SetText(strOrgName.c_str());
	}

	CLabelUI* pHangYe = static_cast<CLabelUI*>(m_PaintManager.FindControl(vbp_label_hangye));
	if (NULL != pHangYe)
	{
		pHangYe->SetText(_T("汽车配件"));
	}

	CLabelUI* pPhone= static_cast<CLabelUI*>(m_PaintManager.FindControl(vbp_label_phone));
	if (NULL != pPhone)
	{
		pPhone->SetText(strPhone.c_str());
	}

	return true;
}

void CViewFoundPartnerInfoPage::CreateWnd()
{
	Create(NULL, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);		
	CenterWindow();	
}

//初始化窗口
void CViewFoundPartnerInfoPage::InitWindow()
{
}