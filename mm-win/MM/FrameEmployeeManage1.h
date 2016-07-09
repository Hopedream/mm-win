#pragma once
#include "mmwinimplbase.h"
#include "FrameEmployeeManage.h"
//struct OrgMemberInfo
//{
//	tstring strLogo;                  //ͷ��
//	tstring strPhone;                 //�绰
//	tstring strNickName;              //�ǳ�
//	bool  brequirement;             //ҵ��-����
//	bool  bengage;                   //ҵ��-��Լ
//	bool  bexecute;                  //ҵ��-ִ��
//	bool  baftersale;                //ҵ��-�ۺ�
//	tstring strAccount;
//	bool   m_bVerify;                 //��֤
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
	bool getOrgInfo();  //��ȡ��ǰ���ڻ���id
	bool getOrgMembers(tstring strOrgId, vector<OrgMemberInfo>& vctMembers);  //��ȡ������Ա
	bool getRecipient();        //��ȡĬ�Ͻ�����
	bool getDuty(OrgMemberInfo &oneInfo);       //��ȡ�����û�ְ��
	bool AddItem(OrgMemberInfo memInfo);
	bool updateItem(OrgMemberInfo memInfo);
	bool onbtnOk();
	bool setRecipient(tstring strPhone);        //����Ĭ�Ͻ�����
	bool setDuty(OrgMemberInfo &oneInfo);
	bool AddMember(tstring strPhone);      //��ӻ�����Ա
	bool MemRemove(tstring strPhone);      //�Ƴ�������Ա
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

