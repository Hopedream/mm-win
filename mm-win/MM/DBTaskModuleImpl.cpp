#include "StdAfx.h"
#include "DBTaskmoduleimpl.h"
#include "IMDB.h"
#include "Controller.h"
#include "UserManager.h"
#include <time.h>

/// dll接口
IDBTaskModule* GetDBTaskModule()
{
	static CDBTaskModuleImpl* p = NULL;
	if (p == NULL)
	{
		p = new CDBTaskModuleImpl;
	}

	return p;
}


/// CDBTaskModuleImpl 实现o
CDBTaskModuleImpl::CDBTaskModuleImpl(void)
{
	::InitializeCriticalSection(&group_cs_);
	::InitializeCriticalSection(&msg_cs_);
	//lee
	CController::Instance()->AddEventHanlder(this);
	//CController::Instance()->GetUserInfoMgr()->AddEventHandler(this);
}

CDBTaskModuleImpl::~CDBTaskModuleImpl(void)
{
	::DeleteCriticalSection(&group_cs_);
	::DeleteCriticalSection(&msg_cs_);
	//lee
	CController::Instance()->RemoveHandler(this);
}


void CDBTaskModuleImpl::AddEventHandler(IDBTaskModuleEvent* pEventHandler)
{
	if (m_ctrEventHandler.insert(pEventHandler).second == false)
	{
		ASSERT(!"已经添加过该事件处理者");
	}
}

void CDBTaskModuleImpl::Release()
{
	delete this;
}

bool CDBTaskModuleImpl::SetDBPath(IM_UID uid)
{
	return IMDB::GetInstance()->SetDBPath(uid);
}

const char * CDBTaskModuleImpl::GetModulePath()
{
    return IMDB::GetInstance()->GetModulePath();
}

void CDBTaskModuleImpl::CloseDB()
{
	IMDB::GetInstance()->CloseDb();
}

bool CDBTaskModuleImpl::GetLoginParams(vector<sLoginUser>& vecLogined)
{
	return IMDB::GetInstance()->GetLoginParams(vecLogined);
}

bool CDBTaskModuleImpl::SaveLoginParams(tstring& strPhoneNum,tstring strPwd,  bool bRemPwd, bool bAutoLogin, __int64 sTime)
{
	return IMDB::GetInstance()->SaveLoginParams(strPhoneNum, strPwd, bRemPwd, bAutoLogin, sTime);
}

bool CDBTaskModuleImpl::SaveChatMessage(IM_UID strUsername, _tChatMessage& stMsg)
{
	return IMDB::GetInstance()->SaveChatMessage(GetUserName(strUsername), stMsg);
}

bool CDBTaskModuleImpl::RemoveChatMessage(IM_UID strUsername)
{
	return IMDB::GetInstance()->RemoveChatMessage(GetUserName(strUsername));
}

bool CDBTaskModuleImpl::RemoveGroupChatMessage(const tstring& rood_id)
{
	return IMDB::GetInstance()->RemoveGroupChatMessage(rood_id);
}

bool CDBTaskModuleImpl::SaveGroupChatMsg(const tstring& strRoomID, _tGroupChatMessage& tGroupMsg)
{
	return IMDB::GetInstance()->SaveGroupChatMessage(strRoomID, tGroupMsg);
}

bool CDBTaskModuleImpl::LoadChatMessage(IM_UID uid, vector<_tChatMessage>& vecMsgs)
{
	return IMDB::GetInstance()->LoadChatMessage(GetUserName(uid), vecMsgs);
}

bool CDBTaskModuleImpl::LoadGroupChatMsg(const tstring& strRoomID, vector<_tGroupChatMessage>& vecGroupMsg)
{
	return IMDB::GetInstance()->LoadGroupChatMsg(strRoomID, vecGroupMsg);
}

bool CDBTaskModuleImpl::LoadLastGroupChatMsg(const tstring& strRoomID, _tGroupChatMessage& groupMsg)
{
	return IMDB::GetInstance()->LoadLastGroupChatMsg(strRoomID, groupMsg);
}

bool CDBTaskModuleImpl::SaveSysMessage(sSysMsg& sysMsg) 
{
//	return true;
	::EnterCriticalSection(&msg_cs_);
	bool ret =  IMDB::GetInstance()->SaveSysMessage(sysMsg);
	::LeaveCriticalSection(&msg_cs_);
	return ret;
}
bool CDBTaskModuleImpl::LoadSysMessage(vector<sSysMsg>& vecSysMsgs)
{
	::EnterCriticalSection(&msg_cs_);
	bool ret = IMDB::GetInstance()->LoadSysMessage(vecSysMsgs);
	::LeaveCriticalSection(&msg_cs_);	
	return ret;
}
bool CDBTaskModuleImpl::GetLastSysMsg(sSysMsg& sysMsg)
{
	::EnterCriticalSection(&msg_cs_);
	bool ret = IMDB::GetInstance()->GetLastSysMsg(sysMsg);
	::LeaveCriticalSection(&msg_cs_);
	return ret;
}
bool CDBTaskModuleImpl::UpdateSysMsgStatus(time_t time, bool bProcessed)
{
	::EnterCriticalSection(&msg_cs_);
	bool ret =  IMDB::GetInstance()->UpdateSysMsgStatus(time, bProcessed);
	::LeaveCriticalSection(&msg_cs_);	
	return ret;
}

tstring CDBTaskModuleImpl::GetUserName(IM_UID uid)
{
	tstring strPhone;
	int index = uid.find('@');
	if (index > 0)
	{
		strPhone = uid.substr(0, index);
	}
	else
	{
		strPhone = uid;
	}

	return strPhone;
}
//获取最后一次的聊天记录
bool CDBTaskModuleImpl::LoadLastMessage(IM_UID uid, _tChatMessage& tLastMsg)
{
	return IMDB::GetInstance()->LoadLastMessage(GetUserName(uid), tLastMsg);
}

bool CDBTaskModuleImpl::SaveUserVCard(IM_UID uid,/*_tUserInfo*/sUserVCard& useinfo)
{
     return IMDB::GetInstance()->SaveUserVCard(GetUserName(uid),useinfo);
}

bool CDBTaskModuleImpl::LoadUserVCard(IM_UID uid,sUserVCard& useinfo)
{
     return IMDB::GetInstance()->LoadUserVCard(GetUserName(uid),useinfo);
}

bool CDBTaskModuleImpl::LoadContactBaseInfo(IM_UID uid,_tUserInfo& useinfo)
{
     return IMDB::GetInstance()->LoadContactBaseInfo(uid,useinfo);
}

bool CDBTaskModuleImpl::SaveRecentGroupItem(RecentGroupItem& oneGroup)
{
	::EnterCriticalSection(&group_cs_);
	bool ret = IMDB::GetInstance()->SaveRecentGroupItem(oneGroup);
	::LeaveCriticalSection(&group_cs_);
	return ret;
}

bool CDBTaskModuleImpl::RemoveFromRecentGroup(const tstring& room_id)
{
	::EnterCriticalSection(&group_cs_);
	bool ret = IMDB::GetInstance()->RemoveFromRecentGroup(room_id);
	::LeaveCriticalSection(&group_cs_);
	return ret;
}

bool CDBTaskModuleImpl::UpdateAvatarInRecentGroup(const tstring& room_id, const tstring& avatar_path)
{
	::EnterCriticalSection(&group_cs_);
	bool ret = IMDB::GetInstance()->UpdateAvatarInRecentGroup(room_id, avatar_path);
	::LeaveCriticalSection(&group_cs_);
	return ret;
}

//最近联系人
bool CDBTaskModuleImpl::SaveRecentFriendList(RECENTITEM& recent /*vector<RECENTITEM>& recentlist*/)
{
	RECENTITEM newItem;
	newItem.strUid = GetUserName(recent.strUid);
	newItem.strTime = recent.strTime;
	return IMDB::GetInstance()->SaveRecentFriendList(newItem);
}

bool CDBTaskModuleImpl::LoadRecentFriendList(vector<RECENTITEM>& recentlist)
{
	return IMDB::GetInstance()->LoadRecentFriendList(recentlist);
}

bool CDBTaskModuleImpl::LoadRecentGroupItem(vector<RecentGroupItem>& recentGroup)
{
	return IMDB::GetInstance()->LoadRecentGroupItem(recentGroup);
}

bool CDBTaskModuleImpl::RemoveRecentItem(IM_UID  uid)
{

	return IMDB::GetInstance()->RemoveRecentItem(GetUserName(uid));
}

bool CDBTaskModuleImpl::AddRecentItem(RECENTITEM& item)
{
	return IMDB::GetInstance()->AddRecentItem(item);
}


void CDBTaskModuleImpl::Initialize()
{
	//lee:
	Client* pClient = CController::Instance()->GetXmppClient();
	if (NULL != pClient)
	{
		//phone number
		tstring strMyAccount = pClient->jid().username();
		IMDB::GetInstance()->SetDBPath(strMyAccount);
	}
}
void CDBTaskModuleImpl::OnConnected()
{
	//Initialize();
}
void CDBTaskModuleImpl::OnConnectFail(const int errCode)
{

}


void CDBTaskModuleImpl::OnLogin(BOOL bSucc,IM_uint32 uErrorCode)
{
	//IMDB::GetInstance()->SetUIN(Self()->Action_Id_Get());
	//IMDB::GetInstance()->SetDBPath(Self()->Action_Id_Get());
}

void CDBTaskModuleImpl::OnChangeStatus(BOOL bSucc,IM_int8 nStatus)
{
	IMDB::GetInstance()->CloseDb();
}

void CDBTaskModuleImpl::OnLogout(IM_uint32 uReason)
{
	IMDB::GetInstance()->CloseDb();
}

void CDBTaskModuleImpl::OnMySelfCardInfo(sUserVCard& oneInfo)
{

}
void CDBTaskModuleImpl::OnVCardReturned(tstring& strAccount, sUserVCard& oneInfo)
{
	IMDB::GetInstance()->SaveUserVCard(GetUserName(strAccount), oneInfo);
}
void CDBTaskModuleImpl::OnStoreVCardResult(tstring& strAccount, bool bSucc)
{

}




bool CDBTaskModuleImpl::SaveCallHistoryList(const tstring& strAccount, const EDialIndicate eCallType)
{
	CallHistoryItem newItem;
	newItem.strUid = GetUserName(strAccount);
	newItem.u32Time = time(NULL);
	newItem.eDialIndicate = eCallType;
	return IMDB::GetInstance()->SaveCallHistoryFriendList(newItem);
}

bool CDBTaskModuleImpl::LoadCallHistoryList(vector<CallHistoryItem>& recentlist)
{
	return IMDB::GetInstance()->LoadCallHistoryList(recentlist);
}

bool CDBTaskModuleImpl::RemoveCallHistoryItem(IM_UID  uid)
{
	return true;
	//return IMDB::GetInstance()->RemoveRecentItem(GetUserName(uid));
}

bool CDBTaskModuleImpl::AddCallHistoryItem(CallHistoryItem& item)
{
	return IMDB::GetInstance()->AddCallHistoryItem(item);
}

bool CDBTaskModuleImpl::SaveSendMsgHotKey( int keyMode )
{
	return IMDB::GetInstance()->SaveSendMsgHotKey(keyMode);
}

int CDBTaskModuleImpl::GetSendMsgHotKey( )
{
	return IMDB::GetInstance()->GetSendMsgHotKey();
}

bool CDBTaskModuleImpl::IsFirstLogin()
{
	return IMDB::GetInstance()->IsFirstLogin();
}

