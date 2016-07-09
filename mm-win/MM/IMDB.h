#pragma once

#include "DB_define.h"
#include "CppSQLite3.h"
#include "tchar.h"
#include "..\include\DBTaskModule.h"
#include "..\Common\UserInfoDef.h"


#ifdef _UNICODE
	#include  <xstring>
	#include <sstream>
	#define TSTRING std::wstring
	#define TSTRING_OSTREAM std::wostringstream
	#ifdef TCHAR
		#undef TCHAR
		#define TCHAR wchar_t
	#endif
#else
	#include <string>
	#include <sstream>
	#define TSTRING std::tstring
	#define TSTRING_OSTREAM std::ostringstream
	#ifdef TCHAR
		#undef TCHAR
		#define TCHAR char
	#endif
#endif

using namespace std;




class IMDB
{
public:
	IMDB(void);
	virtual ~IMDB(void);
	IMDB(IM_UID uid);

public:
	bool IsFirstLogin();
	bool SetDBPath(IM_UID uid);
	const char * GetModulePath();
	static IMDB* GetInstance();
	static void ReleaseInstance();

	bool GetLoginParams(vector<sLoginUser>& );

	bool SaveLoginParams(tstring& strPhoneNum, tstring strPwd, bool bRemPwd, bool bAutoLogin,__int64 sTime);

	bool SaveChatMessage(IM_UID uid, _tChatMessage& tMsg);

	bool RemoveChatMessage(IM_UID uid);

	bool SaveGroupChatMessage(const tstring& strRoomId, _tGroupChatMessage& tGroupMsg);

	bool RemoveGroupChatMessage(const tstring& room_id);

	bool SaveSysMessage(sSysMsg& sysMsg);
	
	bool LoadSysMessage(vector<sSysMsg>& );

	bool GetLastSysMsg(sSysMsg& sysMsg);

	bool UpdateSysMsgStatus(time_t time, bool bProcessed);

	bool SaveUserVCard(IM_UID uid,sUserVCard& usrinfo);


	bool UpdateUserInfo(CppSQLite3DB& db,IM_UID uid,sUserVCard & usrinfo);

	bool UpdateUserExtendInfo(CppSQLite3DB& db,IM_UID uid,_tDetailedUserInfo & extendinfo);

	bool UpdateContactInfo(CppSQLite3DB& db,IM_UID uid,_tUserInfo & usrinfo);

	bool UpdateContactExtendInfo(CppSQLite3DB& db,IM_UID uid,_tDetailedUserInfo & extendinfo);

	bool AddUserInfo(CppSQLite3DB& db,IM_UID uid,/*_tUserInfo*/sUserVCard & usrinfo);

	bool AddUserExtendInfo(CppSQLite3DB& db,IM_UID uid,_tDetailedUserInfo & extendinfo);

	bool AddContactInfo(CppSQLite3DB& db,IM_UID uid,_tUserInfo & usrinfo);

	bool AddContactExtendInfo(CppSQLite3DB& db,IM_UID uid,_tDetailedUserInfo & extendinfo);

	bool BeExistUserInfo(CppSQLite3DB& db,IM_UID uid);

	bool BeExistContacter(CppSQLite3DB& db,IM_UID uid);

	bool BeExistUserExtendInfo(CppSQLite3DB& db,IM_UID uid);

	bool BeExistContacterExtendInfo(CppSQLite3DB& db,IM_UID uid);

	bool BeExistRecentListItem(CppSQLite3DB& db,IM_UID uid);

	bool BeExistRecentGroupItem(CppSQLite3DB& db,const tstring& strRoomID);


	bool LoadGroupChatMsg(const tstring& strRoomID, vector<_tGroupChatMessage>& vecGroupMsgs);
	bool LoadLastGroupChatMsg(const tstring& strRoomID, _tGroupChatMessage& groupMsg);

	bool LoadChatMessage(IM_UID uid, vector<_tChatMessage>& vecMsgs);
	bool LoadLastMessage(IM_UID uid, _tChatMessage& lastMsg);
	bool LoadUserVCard(IM_UID uid,sUserVCard& usrinfo);

	bool LoadContactBaseInfo(IM_UID uid,_tUserInfo& usrinfo);


	bool PrepareTableGroupRecent(CppSQLite3DB& db);

	bool SaveRecentGroupItem(RecentGroupItem& oneGroup);

	bool RemoveFromRecentGroup(const tstring& room_id);

	bool UpdateAvatarInRecentGroup(const tstring& room_id, const tstring& avatar_path);
	
	bool LoadRecentGroupItem(vector<RecentGroupItem>& recentGroup);

	//最近联系人
	bool PrepareTableRecentList(CppSQLite3DB& db);

	bool SaveRecentFriendList(/*vector<RECENTITEM>& recentlist*/RECENTITEM& recent );

	bool LoadRecentFriendList(vector<RECENTITEM>& recentlist);

	bool RemoveRecentItem(IM_UID  uid);

	bool AddRecentItem(RECENTITEM& item);

	bool UpdateRecentItemTime(RECENTITEM& item);

	bool AddRecentItem(CppSQLite3DB& db,RECENTITEM& item);

	bool AddRecentGroupItem(CppSQLite3DB& db, RecentGroupItem& item);

	bool RemoveRecentGroupItem(CppSQLite3DB& db, const tstring& room_id);

	bool UpdateAvatarInRecentGroupItem(CppSQLite3DB& db, const tstring& room_id, const tstring& avatar_path);

	bool UpdateRecentItemTime(CppSQLite3DB& db,RECENTITEM& item);

	bool SaveCallHistoryFriendList(CallHistoryItem& recent );
	bool LoadCallHistoryList(vector<CallHistoryItem>& recentlist);
	bool AddCallHistoryItem(CallHistoryItem& item);
	bool SaveSendMsgHotKey( int keyMode );
	int GetSendMsgHotKey();
	void CloseDb();
private:
	bool PrepareTableTalkMsg(CppSQLite3DB &db,tstring tablename) ;
	bool PrepareTableGroupMsg(CppSQLite3DB &db, tstring strRoomTableName);
	bool PrepareTableContacts(CppSQLite3DB& db) ;
	bool PrepareTableSysMsg(CppSQLite3DB& db);

	CppSQLite3DB & OpenTableTalkMsg();
	CppSQLite3DB & OpenTableContacts();
	CppSQLite3DB & OpenTableOptional();
	CppSQLite3DB & OpenTableGlobalOptional();
	bool PrepareTableCallHistoryList(CppSQLite3DB& db);
	bool BeExistLoginParamItem(CppSQLite3DB& db,IM_UID uid);
	bool BeExistCallHistoryItem(CppSQLite3DB& db,IM_UID uid);	
	bool RemoveCallHistoryItem(IM_UID uid);	
	bool AddCallHistoryItem(CppSQLite3DB& db,CallHistoryItem& item);
	bool AddLoginParams(CppSQLite3DB& db, tstring& strPhoneNum,tstring&strPwd, bool bRemPwd, bool bAutoLogin,__int64 sTime);
	bool UpdateCallHistoryTime(CallHistoryItem& item);
	bool UpdateCallHistoryTime(CppSQLite3DB& db,CallHistoryItem& item);
	bool UpdateLoginParams(CppSQLite3DB& db, tstring strPhoneNum, tstring strPwd, bool bRemPwd, bool bAutoLogin, __int64 nTime);

private:

	//lee:有些函数的局部变量os在函数退出的时候，会crash。可以使用m_os;
	CppSQLite3Query m_query; 
	//ostringstream m_os;
	tstring    m_ModulePath;
	tstring    m_ModuleUinPath;
	tstring	  m_strUserPath;
	char      m_buf[2000];
	static IMDB* m_pInstance;

	CppSQLite3DB m_TalkMsgDb;
	CppSQLite3DB m_OptionalDb;
	CppSQLite3DB m_ContactDb;
	CppSQLite3DB m_GlobalOptionalDb;
	bool m_bFirstLogin;
};
