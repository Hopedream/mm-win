#pragma once
#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"
#include "RoomMgrEvent_i.h"

#define WM_UNSELECT_CHECKBOX	WM_USER+510
#define WM_STEP2_CLOSED			WM_USER+511


/**
1.���û��Ⱥ������ʾΪ�����͡���ť��ProjectListView.h,ProjectListPage.h
2.�����Ⱥ��ֻ������³�Ա�����ڳ�ʼ����ʱ����Ҫselect���Ѿ���Ⱥ��Ա��checkBox��GroupMemberView.h
3.�������1�����ǣ�2.ֻҪ��m_strRoomIDTobeAddNewMember�Ƿ�Ϊ�գ����Ϊ�գ�1������ǿգ�2
*/
class CTileLayoutLocalParticipant;
class CTileLayoutDestOrgList;
class CUIOrgMembersList;
class CTileLayoutDestEmployeeList;
class CTileLayoutDestEmployeeSelectedList;
class CAddNewGroupMember : public WindowImplBase
						  , public CRoomManagerEventBase
{
public:
	CAddNewGroupMember(CPaintManagerUI* ppm = NULL);
	virtual ~CAddNewGroupMember();

	//IRoomMgrEvent
	virtual void OnGetRoomNameSuccess(const std::tstring& strUniqueRoom);
	virtual void OnCreateRoomResult(bool bSucc);

	//ע�⣺���������const���ͻᱨ˵������Ȼ�����ࡣ
	virtual LPCTSTR GetWindowClassName()const	{	return _T("AddGroupMember_class");		}
	virtual CDuiString GetSkinFolder()			{	return _T("..\\MM\\res\\");				}
	virtual CDuiString GetSkinFile()			{	return "groupchat_addnewmember.xml";	}
	virtual void OnFinalMessage( HWND hWnd )	{	delete this;							}
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CControlUI* CreateControl(LPCTSTR pstrClass);

	//������Ϊ��֧�����Ⱥ��Ա�����ӵĸ�������
	void SetShowMode(bool bCreateNewRoom)
	{
		m_bCreateNewRoom = bCreateNewRoom;//���m_bCreateNewRoom=false==>��Ӳ����ˣ�����ʾ�Է�����Ϣ��������ʾ�Է�������
	}
	void SetCurrentRoomInfo(tstring& strRoomID)
	{
		m_strRoomIDTobeAddNewMember = strRoomID;
	}
	//

	void CreateWnd();
	bool Initialize();
	void SetResProperty(bool bAskPrice, tstring& strProjectID, tstring& strProjName);
	void SetForegroundWnd();
protected:
	//tstring GetAccountByItemID(int nItemID);
	void OnAddNewGroupMember();
	void OnPreCreateGroupRoom();
	void OnParticipantSelected();
	void HandleItemClickEvent( TNotifyUI& msg );
	void HandleListItemSelected(TNotifyUI& msg );
	void InitialLocalParticipant();
	void InitialDestOrgList();
	bool IsAllowedToSelected(tstring& strOrgID, tstring& strErrorMsg);
	bool GetDefaultAccountByOrgID(tstring& strOrgID, tstring& strAccount);
	void SendInitPackItem(tstring& strProjID);
protected:
	bool m_bAskPrice;
	
	CPaintManagerUI* m_pPaintManager;
	//����������List
	CTileLayoutLocalParticipant* m_pLocalParticipant;
	//CUIOrgMembersList*			 m_pOrgMemberList;
	CTileLayoutDestOrgList*		 m_pDestOrgList;
	CTileLayoutDestEmployeeList* m_pOppositeEmployeList;
	CTileLayoutDestEmployeeSelectedList* m_pOppositeSelectedEmployeeList;

	//��������֯�����г�Ա
	vector<sFriendListItem*> m_vecMyGroupMembser;

	tstring m_strCurrRoomID;
	tstring m_strCurrProjectID;
	tstring m_strCurrProjName;
	//�ҵĺ����б�
	//vector<sFriendListItem> m_vecAllFriends;

	bool m_bCreateNewRoom;
	tstring m_strRoomIDTobeAddNewMember;
};