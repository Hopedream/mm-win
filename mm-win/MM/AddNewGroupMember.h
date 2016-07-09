#pragma once
#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"
#include "RoomMgrEvent_i.h"

#define WM_UNSELECT_CHECKBOX	WM_USER+510
#define WM_STEP2_CLOSED			WM_USER+511


/**
1.如果没有群，就显示为“发送”按钮：ProjectListView.h,ProjectListPage.h
2.如果有群，只是添加新成员，就在初始化的时候，需要select上已经是群成员的checkBox：GroupMemberView.h
3.如何区分1，还是，2.只要看m_strRoomIDTobeAddNewMember是否为空，如果为空：1，如果非空：2
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

	//注意：如果不加上const，就会报说该类仍然是虚类。
	virtual LPCTSTR GetWindowClassName()const	{	return _T("AddGroupMember_class");		}
	virtual CDuiString GetSkinFolder()			{	return _T("..\\MM\\res\\");				}
	virtual CDuiString GetSkinFile()			{	return "groupchat_addnewmember.xml";	}
	virtual void OnFinalMessage( HWND hWnd )	{	delete this;							}
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CControlUI* CreateControl(LPCTSTR pstrClass);

	//以下是为了支持添加群成员而增加的辅助函数
	void SetShowMode(bool bCreateNewRoom)
	{
		m_bCreateNewRoom = bCreateNewRoom;//如果m_bCreateNewRoom=false==>添加参与人，不显示对方的信息。否则显示对方参与人
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
	//本方参与人List
	CTileLayoutLocalParticipant* m_pLocalParticipant;
	//CUIOrgMembersList*			 m_pOrgMemberList;
	CTileLayoutDestOrgList*		 m_pDestOrgList;
	CTileLayoutDestEmployeeList* m_pOppositeEmployeList;
	CTileLayoutDestEmployeeSelectedList* m_pOppositeSelectedEmployeeList;

	//我所在组织的所有成员
	vector<sFriendListItem*> m_vecMyGroupMembser;

	tstring m_strCurrRoomID;
	tstring m_strCurrProjectID;
	tstring m_strCurrProjName;
	//我的好友列表
	//vector<sFriendListItem> m_vecAllFriends;

	bool m_bCreateNewRoom;
	tstring m_strRoomIDTobeAddNewMember;
};