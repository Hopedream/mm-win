#pragma once
#include "mmwinimplbase.h"


class CFrameChangePassword :
	public MMWindowImplBase
{
public:
	static CFrameChangePassword* Instance();
	CFrameChangePassword();
	~CFrameChangePassword(void);

public: 
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void showFocus();
private:
	void GetVerifyCode();
	void changePasswd();
	void clearDlg();

private:
	static CFrameChangePassword* m_pInstance;
	CLabelUI *m_pLableTip;
	CEditUI* m_pConfirmPasswdEditUI;
	CEditUI* m_pPasswdEditUI;
	CEditUI* m_pPinCodeEditUI;
	int m_nPinLifecycle;
	CButtonUI* m_pBtnGetPinCode;
	tstring m_strPhone;
};

