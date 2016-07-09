#pragma once
#include "mmwinimplbase.h"
#include "FrameEmployeeManage.h"
//struct OrgMemberInfo
//{
//	tstring strLogo;                  //头像
//	tstring strPhone;                 //电话
//	tstring strNickName;              //昵称
//	bool  brequirement;             //业务-需求
//	bool  bengage;                   //业务-订约
//	bool  bexecute;                  //业务-执行
//	bool  baftersale;                //业务-售后
//	tstring strAccount;
//	bool   m_bVerify;                 //验证
//
//};


class CFrameEmployeeManage1 :
	public MMWindowImplBase, public CUserManagerEventBase
{
public:
	CFrameEmployeeManage1();
	~CFrameEmployeeManage1(void);
	static CFrameEmployeeManage1* Instance();

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
	virtual LRESULT OnMouseWheel(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMouseMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
private:
	bool getOrgInfo();  //获取当前所在机构id
	bool getOrgMembers(tstring strOrgId, vector<OrgMemberInfo>& vctMembers);  //获取机构成员
	bool getRecipient();        //获取默认接收人
	bool getDuty(OrgMemberInfo &oneInfo);       //获取单个用户职责
	bool AddItem(OrgMemberInfo memInfo);
	bool updateItem(OrgMemberInfo memInfo);
	bool onbtnOk();
	bool setRecipient(tstring strPhone);        //设置默认接收人
	bool setDuty(OrgMemberInfo &oneInfo);
	bool AddMember(tstring strPhone);      //添加机构成员
	bool MemRemove(tstring strPhone);      //移除机构成员
	bool TransferOwnerPermissions(tstring strPhone);
	void updateWindow();
private:
	static CFrameEmployeeManage1* m_pInstance;
	tstring m_strOid;
	CEditUI* m_pEditPhone;
	CListUI* m_plstEmployee;
	vector<OrgMemberInfo> m_vctMembers;
	tstring m_strRecipient;
	tstring m_strMySelfPhone;
};

