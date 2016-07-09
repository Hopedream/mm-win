#pragma once
#include "mmwinimplbase.h"
#include "../include/UsrManagerEvent_i.h"


struct OrgMemberInfo
{
	tstring strLogo;                  //ͷ��
	tstring strPhone;                 //�绰
	tstring strNickName;              //�ǳ�
	bool  brequirement;             //ҵ��-����
	bool  bengage;                   //ҵ��-��Լ
	bool  bexecute;                  //ҵ��-ִ��
	bool  baftersale;                //ҵ��-�ۺ�
	tstring strAccount;
	bool   m_bVerify;                 //��֤

};

class CFrameEmployeeManage :
	public MMWindowImplBase , public CUserManagerEventBase
{
public:
	CFrameEmployeeManage();
	static CFrameEmployeeManage* Instance();
	~CFrameEmployeeManage(void);
	bool createDefaultOrg();             // ��ʱ�ӿڣ�����һ��Ĭ�ϵ�Org;
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
	bool getRecipient();        //��ȡĬ�Ͻ�����
	bool setRecipient(tstring strPhone);        //����Ĭ�Ͻ�����
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

