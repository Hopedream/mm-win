#pragma once
#include "mmwinimplbase.h"
#include "../include/UsrManagerEvent_i.h"


struct OrgMemberInfo
{
	tstring strLogo;                  //头像
	tstring strPhone;                 //电话
	tstring strNickName;              //昵称
	bool  brequirement;             //业务-需求
	bool  bengage;                   //业务-订约
	bool  bexecute;                  //业务-执行
	bool  baftersale;                //业务-售后
	tstring strAccount;
	bool   m_bVerify;                 //验证

};

class CFrameEmployeeManage :
	public MMWindowImplBase , public CUserManagerEventBase
{
public:
	CFrameEmployeeManage();
	static CFrameEmployeeManage* Instance();
	~CFrameEmployeeManage(void);
	bool createDefaultOrg();             // 临时接口，创建一个默认的Org;
	bool getOrgInfo(tstring& strOrgId);
	bool getOrgMembers(tstring strOrgId, vector<OrgMemberInfo>& vctMembers);
	bool AddItem(OrgMemberInfo memInfo);
	void addItems();
	bool AddWaitItem(OrgMemberInfo memInfo);
	bool m_bHaveOrg;
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
	virtual void OnStrangerVCard(tstring& strAccount, sUserVCard& oneInfo);
	virtual void OnVCardReturned(tstring& strAccount, sUserVCard& oneInfo);
	virtual void OnMySelfCardInfo(sUserVCard& oneInfo);
private:
	bool TransferOwnerPermissions(tstring strPhone);
	bool getDuty(OrgMemberInfo &oneInfo);
	bool getRecipient();        //获取默认接收人
	bool setRecipient(tstring strPhone);        //设置默认接收人
	bool setDuty(OrgMemberInfo &oneInfo);
	void onbtnOk();
private:
	static CFrameEmployeeManage* m_pInstance;
	CListUI* m_plstEmployee;
	vector<OrgMemberInfo> m_vctMembers;
	CEditUI* m_pEditPhone;
	tstring m_strOrgId;
	tstring m_strRecipient;
};

