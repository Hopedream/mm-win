#pragma once
#include "mmwinimplbase.h"
#include "../include/UsrManagerEvent_i.h"
#include "FrameEmployeeManage.h"

class CFrameEmployee :
	public MMWindowImplBase, public CUserManagerEventBase
{
public:
	CFrameEmployee();
	~CFrameEmployee(void);
	static CFrameEmployee* Instance();
public:
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void HandleSelectedChangedEvent(TNotifyUI& msg);
	virtual void HandleItemSelectEvent(TNotifyUI& msg);
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();
	virtual void OnVCardReturned(tstring& strAccount, sUserVCard& oneInfo);
	virtual void OnMySelfCardInfo(sUserVCard& oneInfo);

private:
	bool getOrgId();
	bool getOrgMembers(tstring strOrgId, vector<OrgMemberInfo>& vctMembers);  //获取机构成员
	bool getRecipient();        //获取默认接收人
	bool updateItem(OrgMemberInfo memInfo);
	bool AddItem(OrgMemberInfo memInfo);
	bool AddMember(tstring strPhone);      //添加机构成员
private:
	static CFrameEmployee* m_pInstance;
	tstring m_strOid;
	vector<OrgMemberInfo> m_vctMembers;
	CEditUI* m_pEditPhone;
	CListUI* m_plstEmployee;
	tstring m_strRecipient;
};

