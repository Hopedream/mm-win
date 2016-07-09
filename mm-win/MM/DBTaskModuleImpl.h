#pragma once
#include <set>
#include "DB_define.h"
#include "..\include\DBTaskModule.h"
#include "ControllerEvent_i.h"
#include "..\Include\UsrManagerEvent_i.h"

class CDBTaskModuleImpl : public IDBTaskModule
						, public IControllerEvent
						, public CUserManagerEventBase
{
public:
	CDBTaskModuleImpl(void);
	~CDBTaskModuleImpl(void);

	virtual void AddEventHandler(IDBTaskModuleEvent* pEventHandler);
	virtual void Release();

	virtual bool SetDBPath(IM_UID uid);

	virtual const char * GetModulePath();

	virtual void CloseDB();

	virtual bool GetLoginParams(vector<sLoginUser>& );

	virtual bool SaveLoginParams(tstring& strPhoneNum, tstring strPwd,  bool bRemPwd, bool bAutoLogin,__int64 sTime);

	virtual bool SaveChatMessage(IM_UID uid, _tChatMessage& tMsg);

	virtual bool RemoveChatMessage(IM_UID uid);

	virtual bool RemoveGroupChatMessage(const tstring& rood_id);

	virtual bool SaveGroupChatMsg(const tstring& strRoomID, _tGroupChatMessage& tGroupMsg);
	
	virtual bool LoadChatMessage(IM_UID uid, vector<_tChatMessage>& vecMsgs);

	virtual bool LoadGroupChatMsg(const tstring& strRoomID, vector<_tGroupChatMessage>& tGroupMsg);

	virtual bool LoadLastGroupChatMsg(const tstring& strRoomID, _tGroupChatMessage& groupMsg);

	virtual bool SaveSysMessage(sSysMsg& sysMsg);
	virtual bool LoadSysMessage(vector<sSysMsg>& );
	virtual bool GetLastSysMsg(sSysMsg& sysMsg);
	virtual bool UpdateSysMsgStatus(time_t time, bool bProcessed);


	virtual bool LoadLastMessage(IM_UID uid, _tChatMessage& tLastMsg);

	virtual bool SaveUserVCard(IM_UID uid,/*_tUserInfo*/sUserVCard& useinfo);

	virtual bool LoadUserVCard(IM_UID uid,sUserVCard& useinfo);

	virtual bool LoadContactBaseInfo(IM_UID uid,_tUserInfo& useinfo);

	virtual bool SaveRecentGroupItem(RecentGroupItem& oneGroup);

	virtual bool RemoveFromRecentGroup(const tstring& room_id);

	virtual bool UpdateAvatarInRecentGroup(const tstring& room_id, const tstring& avatar_path);

	virtual bool SaveRecentFriendList(RECENTITEM& recent /*vector<RECENTITEM>& recentlist*/);

	virtual bool LoadRecentFriendList(vector<RECENTITEM>& recentlist);

	virtual bool LoadRecentGroupItem(vector<RecentGroupItem>& recentGroup);

	virtual bool RemoveRecentItem(IM_UID  uid);

	virtual bool AddRecentItem(RECENTITEM& item);



	virtual void Initialize();

	//IControllerEvent
	virtual void OnConnected();
	virtual void OnConnectFail(const int errCode);

	//CUserManagerEventBase:缓存本地的vcard等信息。
	virtual void OnMySelfCardInfo(sUserVCard& oneInfo);
	virtual void OnVCardReturned(tstring& strAccount, sUserVCard& oneInfo);
	virtual void OnStoreVCardResult(tstring& strAccount, bool bSucc);

	virtual bool IsFirstLogin();
	virtual	void __stdcall OnLogin(BOOL bSucc,IM_uint32 uErrorCode);
	virtual	void __stdcall OnChangeStatus(BOOL bSucc,IM_int8 nStatus);
	virtual	void __stdcall OnLogout(IM_uint32 uReason);


	bool SaveCallHistoryList(const tstring& friendItem, const EDialIndicate eCallType);
	bool LoadCallHistoryList(vector<CallHistoryItem>& recentlist);
	bool RemoveCallHistoryItem(IM_UID uid);
	bool AddCallHistoryItem(CallHistoryItem& item);

	virtual bool SaveSendMsgHotKey(int keyMode) ;
	virtual int GetSendMsgHotKey();

protected:
	/**
	* @uid : eg:189@test-d ---> 189
	*/
	tstring GetUserName(IM_UID uid);

private:
	typedef std::set<IDBTaskModuleEvent*> CCtrEventHandler;
	CCtrEventHandler m_ctrEventHandler;

	CRITICAL_SECTION group_cs_;
	CRITICAL_SECTION msg_cs_;

};