#pragma once

#include "MMWinImplBase.h"
#include "CallRecentContactUI.h"
#include "UICallFriends.h"
#include "UIListCommonDefine.h"
#include "DBTaskModuleImpl.h"
#include "UCSNetServiceCallBackInterface.h"
#include <vector>
using namespace std;


class CCallFrame : public MMWindowImplBase
{
public:	
	static CCallFrame* Instance();
	void InitalFriendList(vector<sFriendListItem>& lsAllFriendsList, bool bForceUpdate = false);
public: 
	virtual void HandleSetFocusEvent(TNotifyUI& msg);
	virtual void HandleKillFocusEvent(TNotifyUI& msg);
	virtual void HandleTextChangedEvent(TNotifyUI& msg);
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();

protected:  		
	CCallFrame();	
	~CCallFrame(void);
	virtual void HandleClickEvent(TNotifyUI& msg);
	void InitFriendsList();
	void InitCallHistoryList();
	void UpdateSwitchBtnBkColor(bool bClickCallHistoryBtn);
	void HandleItemClickEvent( TNotifyUI& msg );
	void SetFocusRearchEditBox(bool bFocus);	
	bool IsFriendOnline(const tstring strAccount);
	void UpdateCallHistory(const sFriendListItem friendItem, EDialIndicate eDialIndicate = eDialCallOut);
	bool GetFriendItem(int itemIdx, sFriendListItem& friendItem);
	void LauchNetCall(int itemIdx, UCS_DIALTYPE eDialType);
private:
	vector<sFriendListItem> m_lsFriendList;
	CButtonUI* m_pBtnCallHistoryView;
	CButtonUI* m_pBtnFriendListView;
	CRecentCallContactUI* m_pCallHistoryListUI;
	CCallFriendsUI* m_pFriendListUI;
	CTabLayoutUI* m_pTabLayout;	
	CEditUI* m_pRearchEdit;
	static CCallFrame* m_pInstance;
	bool		m_bIsInitialFriends;
	map<tstring, DuiLib::Node*> m_mapOrg2Node;
};
