#include "StdAfx.h"
#include "systemSetting.h"
#include "Controller.h"
//#include "../gloox-1.0.14/src/jid.h"
#include "UIDefine.h"
#include "FileHelper.h"
#include "MagicKHelper.h"
#include "StringHelper.h"


#define SHOW_SYSINFO_ITEM 0
#define EDIT_PERSONINFO_ITEM 1

#define WM_FETCH_VCARD (WM_USER+20)
#define WM_STORE_VCARD (WM_USER+21)

DuiLib::CDuiString CSystemSetDlg::GetSkinFile()
{
	return _T("systemConfig.xml");
}

LRESULT CSystemSetDlg::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	if (WM_FETCH_VCARD == uMsg)
	{
		m_pMyLogoCtrl->SetBkImage(m_myVcard.strPhotoPath.c_str());
		UpdateVcardInfo();
	}
	else if (WM_STORE_VCARD == uMsg)
	{
		if (true == (bool)wParam)
		{
			UpdateVcardInfo();
		}
		else
		{

			//JID myJid(m_strMyAccount.GetData());
			CController::Instance()->GetUserInfoMgr()->FetchVCardByAccount(tstring(m_strMyAccount.GetData()));//FetchvcardByJid(myJid);
		}
	}
	return 0;
}

void CSystemSetDlg::HandleClickEvent( TNotifyUI& msg )
{
	if (msg.pSender == m_pBtnEditPersonInfo)
	{
		m_pTabLayout->SelectItem(EDIT_PERSONINFO_ITEM);
		InitVCardEditPage();
	}
	else if (msg.pSender == m_pBtnSavePersonInfo)
	{
		SavePersonInfo();
		m_pTabLayout->SelectItem(SHOW_SYSINFO_ITEM);		
	}
	else if (msg.pSender == m_pBtnUpdateLogo)
	{
		UpdateLogo();
	}
}

void CSystemSetDlg::InitVCardEditPage()
{
	m_pMyLogoUpdate->SetBkImage(m_myVcard.strPhotoPath.c_str());
	m_ptxtMyName->SetText(m_myVcard.strUserNickname.c_str());
	m_ptxtMyPhone->SetText(m_myVcard.strPhone.c_str());
	m_ptxtMyAddress->SetText(m_myVcard.strAddr.c_str());
	m_pComboMyGender->SelectItem(0 == m_myVcard.strGender.compare("男")?0:1);
	m_ptxtMyEmailAddr->SetText(m_myVcard.strEmail.c_str());
	m_ptxtMysignature->SetText(m_myVcard.strSignature.c_str());
	m_ptxtMyOrg->SetText(m_myVcard.strOrganization.c_str());
	m_ptxtMyAge->SetText(m_myVcard.strAge.c_str());
}

void CSystemSetDlg::UpdateVcardInfo()
{
	//m_pMyLogoCtrl->SetBkImage(m_myVcard.strPhotoPath.c_str());
	m_pMyNickNameLabel->SetText(m_myVcard.strUserNickname.c_str());
	m_pMyPhoneLabel->SetText(m_myVcard.strPhone.c_str());
	m_pMySignature->SetText(m_myVcard.strSignature.c_str());
	m_pMyGenderLabel->SetText(m_myVcard.strGender.c_str());
	m_pMyEmailLabel->SetText(m_myVcard.strEmail.c_str());
	m_pMyAddressLabel->SetText(m_myVcard.strAddr.c_str());
	m_pMyOrgLabel->SetText(m_myVcard.strOrganization.c_str());
	m_pMyAgeLabel->SetText(m_myVcard.strAge.c_str());		
}


void CSystemSetDlg::InitWindow()
{
	m_pMyLogoCtrl = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("ctrlMyLogo")));
	m_pMyNickNameLabel= static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelMyNickName")));
	m_pMySignature = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelMyIdiograph")));
	m_pMyGenderLabel = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelMyGender")));	
	m_pMyPhoneLabel = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelMyPhoneNumber")));	
	m_pMyAgeLabel = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelMyAge")));
	m_pMyEmailLabel = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelMyEmailAddr")));
	m_pMyOrgLabel = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelMyOrg")));	
	m_pMyAddressLabel = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("labelMyAddress")));		

	m_pMyLogoUpdate = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("ctrlMyLogoUpdate")));
	m_ptxtMyName = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("txtMyName")));
	m_pComboMyGender = static_cast<CComboUI*>(m_PaintManager.FindControl(_T("comboxGender")));
	m_ptxtMyAge = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("txtMyAge")));
	m_ptxtMyOrg = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("txtMyOrg")));
	m_ptxtMyPhone = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("txtMyPhone")));
	m_ptxtMyEmailAddr = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("txtMyEmailAddr")));
	m_ptxtMyAddress = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("txtMyAddress")));
	m_ptxtMysignature = static_cast<CEditUI*>(m_PaintManager.FindControl(_T("txtMysignature")));
	m_pBtnUpdateLogo = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnUpdatgeLogo")));
	m_pBtnSavePersonInfo = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnSavePersonEditInfo")));
	m_pBtnEditPersonInfo = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnEditPersonInfo")));
	m_pTabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switchLayout")));
	//JID myJid(m_strMyAccount.GetData());
	CController::Instance()->GetUserInfoMgr()->FetchVCardByAccount(tstring(m_strMyAccount.GetData()));//FetchvcardByJid(myJid);
}

void CSystemSetDlg::CreateDlg()
{
	Create(NULL, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);
	CenterWindow();
	ShowWindow(true);
}

void CSystemSetDlg::OnMySelfCardInfo( sUserVCard& oneInfo )
{
	//sUserVCard* pMyVcard = new sUserVCard(oneInfo);
	m_myVcard = oneInfo;
	PostMessage(WM_FETCH_VCARD);
}

void CSystemSetDlg::OnStoreVCardResult( tstring& strAccount, bool bSucc )
{
	PostMessage(WM_STORE_VCARD,bSucc);
}

CSystemSetDlg::CSystemSetDlg(CDuiString strAccount)
{
	m_strUpdateLogo = (_T(""));
	m_strMyAccount = strAccount;
	CController::Instance()->GetUserInfoMgr()->AddEventHandler((IUserManagerEvent*)this);
}

CSystemSetDlg::~CSystemSetDlg()
{
	CController::Instance()->GetUserInfoMgr()->RemoveEventHandler((IUserManagerEvent*)this);
}


void CSystemSetDlg::SavePersonInfo()
{
	//sUserVCard myVcard;
	m_myVcard.strPhotoPath =  m_strUpdateLogo.IsEmpty()? m_pMyLogoUpdate->GetBkImage(): m_strUpdateLogo;
	m_myVcard.strUserNickname =  m_ptxtMyName->GetText();
	m_myVcard.strPhone =  m_ptxtMyPhone->GetText();
	m_myVcard.strAddr =  m_ptxtMyAddress->GetText().GetData();
	m_myVcard.strGender =  m_pComboMyGender->GetCurSel() ? "女" : "男";
	m_myVcard.strEmail =  m_ptxtMyEmailAddr->GetText();
	m_myVcard.strSignature =  m_ptxtMysignature->GetText().GetData();
	m_myVcard.strAge = m_ptxtMyAge->GetText().GetData();
	m_myVcard.strOrganization = m_ptxtMyOrg->GetText().GetData();
	CMagicKHelper::Instance()->GetPhotoBinval(m_myVcard.strPhotoPath, m_myVcard.strPhotoBinary);
	m_myVcard.strPhotoType = "image/jpeg";
	CController::Instance()->GetUserInfoMgr()->StoreMyVCard(m_myVcard);
}

void CSystemSetDlg::UpdateLogo()
{
	vector<tstring> lstFiles;
	if (getSelectedFiles(lstFiles,"JPG格式图片\0*.jpg\0JPEG格式图片\0\*.jpeg\0BMP格式图片\0*.bmp\0All\0*.*\0"));
	{		
		if (0 == lstFiles.size())
		{
			return;
		}
		tstring strPhonePath = lstFiles.front();

		m_strUpdateLogo = CFileHelper::GetHeadCacheDir() + "tmp.jpeg" ;		
		CMagicKHelper::Instance()->ResizeImage(strPhonePath, m_strUpdateLogo.GetData(), IMG_PIXEL_LIMIT, IMG_PIXEL_LIMIT);
		m_pMyLogoUpdate->SetBkImage(strPhonePath.c_str());	
		m_pMyLogoCtrl->SetBkImage(strPhonePath.c_str());	
	}
}