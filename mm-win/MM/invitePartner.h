 #pragma once
#include "StdAfx.h"
#include "MMWinImplBase.h"

class CInvitePartner : public MMWindowImplBase
{
public:
	CInvitePartner();
	~CInvitePartner();

public:
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);

	virtual CDuiString GetSkinFile();
	void InitWindow();
private:
	void SetTabView(int idx);
	void OnClickInvitePartner();
	bool InvitePartner(const tstring strPhoneNo, CDuiString& strMsg);
	bool ValidatePhoneNo(const tstring& strPhoneNo);
private:
	CEditUI* m_pTxtPhoneNo1;
	CEditUI* m_pTxtPhoneNo2;
	CEditUI* m_pTxtPhoneNo3;
	CButtonUI* m_pBtnClearPhoneNo1;
	CButtonUI* m_pBtnClearPhoneNo2;
	CButtonUI* m_pBtnClearPhoneNo3;
	CButtonUI* m_pBtnInvitePartner;
	CButtonUI* m_pBtnContinueInvite;

private:
	enum{
		eRegisterFailed = 0,
		eRegisterSuccess = 1,
	};
};
