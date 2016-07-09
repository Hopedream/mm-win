#pragma once
#include "MMWinImplBase.h"
#include "../include/UsrManagerEvent_i.h"
#include "UserManager.h"

class CSystemSetDlg : public MMWindowImplBase, public CUserManagerEventBase
{
public:
	CSystemSetDlg(CDuiString strAccount);
	~CSystemSetDlg();
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void OnMySelfCardInfo(sUserVCard& oneInfo) ;
	virtual void OnStoreVCardResult(tstring& strAccount, bool bSucc);


	virtual CDuiString GetSkinFile();
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void InitWindow();
	void CreateDlg();
	void UpdateVcardInfo();
	void SavePersonInfo();
	void UpdateLogo();
	void InitVCardEditPage();
private:
	CDuiString m_strMyAccount;
	CControlUI* m_pMyLogoCtrl;
	CLabelUI* m_pMyNickNameLabel;
	CLabelUI* m_pMySignature;
	CLabelUI* m_pMyGenderLabel;	
	CLabelUI* m_pMyPhoneLabel;	
	CLabelUI* m_pMyAgeLabel;
	CLabelUI* m_pMyEmailLabel;
	CLabelUI* m_pMyOrgLabel ;	
	CLabelUI* m_pMyAddressLabel;	

	CControlUI* m_pMyLogoUpdate;
	CEditUI* m_ptxtMyName;
	CComboUI* m_pComboMyGender;
	CEditUI* m_ptxtMyAge;
	CEditUI* m_ptxtMyOrg;
	CEditUI* m_ptxtMyPhone;
	CEditUI* m_ptxtMyEmailAddr;
	CEditUI* m_ptxtMyAddress;
	CEditUI* m_ptxtMysignature;
	CButtonUI* m_pBtnUpdateLogo;
	CButtonUI* m_pBtnEditPersonInfo;
	CButtonUI* m_pBtnSavePersonInfo;
	CTabLayoutUI* m_pTabLayout;
	
private:
	sUserVCard m_myVcard;
	CDuiString m_strUpdateLogo;
	enum{
		IMG_PIXEL_LIMIT = 200,
	};
};
