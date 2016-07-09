#pragma once

#include "StdAfx.h"
#include "RoomMgrEvent_i.h"

/*#define WM_GROUPMEM_CLOSED		WM_USER+503*/

class CAddNewGroupMember;
class CTileLayoutGroupMember;
class CGroupMemberView : public WindowImplBase
					   , public CRoomManagerEventBase
{
public:
	CGroupMemberView(CPaintManagerUI* ppm = NULL);
	virtual ~CGroupMemberView();

	void SetMainWndHwnd(HWND hMainWnd)
	{
		m_hMainWnd = hMainWnd;
	}

	virtual void OnNewGroupMemberList(const std::tstring& strRoomID, std::vector<tstring>& vecMemList);

	//注意：如果不加上const，就会报说该类仍然是虚类。
	virtual LPCTSTR GetWindowClassName()const	{	return _T("GroupMember_ListWnd");			}
	virtual CDuiString GetSkinFolder()			{	return _T("..\\MM\\res\\");					}
	virtual CDuiString GetSkinFile()			{	return "groupmember_listwnd.xml";			}
	virtual void OnFinalMessage( HWND hWnd )	{	delete this;								}
	virtual void Notify(TNotifyUI& msg);
	virtual void InitWindow();
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	CControlUI* CreateControl(LPCTSTR pstrClass);


	void CreateWnd();
	bool Initialize(tstring& strRoomID);
protected:
	CPaintManagerUI* m_pPaintManager;
	CTileLayoutGroupMember* m_pGroupMemberList;
	HWND m_hMainWnd;
	//CAddNewGroupMember* m_pAddGroupMemWnd;
	tstring m_strRoomID;
};