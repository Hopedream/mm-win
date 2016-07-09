#pragma once
#include "mmwinimplbase.h"
#include "../include/UsrManagerEvent_i.h"
#include "UserManager.h"

class CFrameUserInfo :
	public MMWindowImplBase,  public CUserManagerEventBase
{
public:
	CFrameUserInfo();
	~CFrameUserInfo(void);
	static CFrameUserInfo* Instance();
public:
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void HandleItemSelectEvent(TNotifyUI& msg);
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();

	virtual void OnMySelfCardInfo(sUserVCard& oneInfo);
	virtual void OnStoreVCardResult(tstring& strAccount, bool bSucc);

private:
	void onSelectAvatar();
	void SavePersonInfo();
	void updateinfo();
public:
	
	
private:
	tstring m_strAvatar;
	tstring m_strAvatarType;
	CDuiString m_strUpdateLogo;
	static CFrameUserInfo* m_pInstance;
	sUserVCard m_myUserInfo;
	CButtonUI *m_pBtnSelect;                        //头像
	CEditUI   *m_pEdtName;                         //名称
	CComboUI *m_pCombVocation;                     //行业     
};

