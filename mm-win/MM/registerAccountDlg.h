 #pragma once
#include "StdAfx.h"
#include "MMWinImplBase.h"

#define REG_PAGE_VIEW 0
#define REG_COMPLETE_VIEW 1
#define DLG_TYPE_RESET_PASSWORD 2
class CRegisterAccountDlg : public MMWindowImplBase
{
public:
	CRegisterAccountDlg(int dlg_type);
	~CRegisterAccountDlg();

public:
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);
	virtual void CreateWnd(HWND hWnd = NULL);

	virtual CDuiString GetSkinFile();
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void InitWindow();
private:
	void RegisterUser();
	bool CheckRegisterInfo();
	void GetVerifyCode();
	void GetVerifyCodeFromCss();//从CSS获得验证码
	void InitRegDlg();
	void EnableFetchPinCodeBtn(bool bEnabled);
	void SetRegTabView(int idx);
	void SwitchPasswdMode();
	void ResetPassword();
private:
	CEditUI* m_pUserNameEditUI;
	CEditUI* m_pPasswdEditUI;
	CEditUI* m_pConfirmPasswdEditUI;
	CEditUI* m_pRegisterTipEditUI;
	CEditUI* m_pPinCodeEditUI;
	CButtonUI* m_pBtnGetPinCode; 
	CCheckBoxUI* m_pAcceptAgree;
	CLabelUI* m_pLabelPhoneTip;
	CLabelUI* m_pLabelPinCode;
	CLabelUI* m_pLabelPasswdTip;
	CLabelUI* m_pLabelRePasswdTip;
	CControlUI* m_pCtrlCompleteRegIcon;
	CLabelUI* m_pLabelRegResult;
	CLabelUI* m_pLabelRegResultDescription;
	CButtonUI* m_pBtnPasswdMode;
	CButtonUI* m_pBtnCompleteReg;
	int m_nPinLifecycle;
	static bool m_bActive;
	int dlg_type_;
private:
	enum{
		eRegisterFailed = 0,
		eRegisterSuccess = 1,
	};
};
