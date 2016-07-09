#pragma once
#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"
#include "RoomMgrEvent_i.h"
#include "CreateNewProjectStep1.h"

#define WM_UNSELECT_CHECKBOX	WM_USER+510
#define WM_STEP2_CLOSED			WM_USER+511

class CTileLayoutLocalParticipant;
class CTileLayoutDestOrgList;
class CUIOrgMembersList;
class CTileLayoutDestEmployeeList;
class CTileLayoutDestEmployeeSelectedList;
class CCreateNewProjStep2 : public WindowImplBase
						  , public CRoomManagerEventBase
{
public:
	CCreateNewProjStep2(CPaintManagerUI* ppm = NULL);
	virtual ~CCreateNewProjStep2();

	void SetParentWnd(CCreateNewProjStep1/*WindowImplBase*/* pParent)
	{
		m_pParent = pParent;
	}

	//IRoomMgrEvent
	virtual void OnGetRoomNameSuccess(const std::tstring& strUniqueRoom);
	virtual void OnCreateRoomResult(bool bSucc);

	//注意：如果不加上const，就会报说该类仍然是虚类。
	virtual LPCTSTR GetWindowClassName()const	{	return _T("CreateNewProjStep2");		}
	virtual CDuiString GetSkinFolder()			{	return _T("..\\MM\\res\\");				}
	virtual CDuiString GetSkinFile()			{	return "create_new_project_step2.xml";	}
	virtual void OnFinalMessage( HWND hWnd )	{	delete this;							}
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CControlUI* CreateControl(LPCTSTR pstrClass);

	void CreateWnd();
	bool Initialize(/*sUserVCard* pUserInfo*/);
	void SetResProperty(bool bAskPrice, tstring& strProjectID, tstring& strProjName, tstring& strPackID);
	void SetForegroundWnd();
protected:
	//tstring GetAccountByItemID(int nItemID);
	void OnPreCreateGroupRoom();
	void OnParticipantSelected();
	void HandleItemClickEvent( TNotifyUI& msg );
	void HandleListItemSelected(TNotifyUI& msg );
	void InitialLocalParticipant();
	void InitialDestOrgList();
	bool IsAllowedToSelected(tstring& strOrgID, tstring& strErrorMsg);
	bool GetDefaultAccountByOrgID(tstring& strOrgID, tstring& strAccount);

protected:
	bool m_bAskPrice;
	/*WindowImplBase*/CCreateNewProjStep1* m_pParent;

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
	tstring m_strCurrPackID;
	//我的好友列表
	//vector<sFriendListItem> m_vecAllFriends;
};