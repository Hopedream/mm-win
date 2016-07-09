/*
@file:		DBTaskModule.h
@purpose:	本地数据库接口，对外提供各种消息和设置的本地存取
*/
#pragma once

#include <vector>
#include "DB_define.h"
#include "..\Common\UserInfoDef.h"
#include "..\Common\MMDefine.h"

using namespace std;

typedef struct tagRecentItem 
{
	tstring    strUid;
	tstring    strTime;
}RECENTITEM,*PRECENTITEM;

typedef struct tagRecentGroupItem
{
	tstring strRoomID;
	tstring strOrgID;
	tstring strOrgName;
	tstring strParticipant;//myself;
	tstring strGroupNick;
    tstring strAvatarPath;
}RecentGroupItem;



enum EDialIndicate{
	eDialCallOut,
	eDialCallIn,
	eCallBarring,
	eValideCall
};
typedef struct tagCallHistoryItem 
{
	tstring						strUid;
	UINT						u32Time;
	EDialIndicate 		eDialIndicate;
}CallHistoryItem,*PCallHistoryItem;



class IDBTaskModuleEvent;
class IDBTaskModule
{
public:
	virtual void AddEventHandler(IDBTaskModuleEvent* pEventHandler) = 0;	// 添加事件处理者
	virtual void Release() = 0;

	/// 该模块的实际功能接口
	//SaveRecent(vector<uin>) = 0;

	virtual bool SetDBPath(IM_UID uid) = 0;

	virtual void Initialize() = 0;

	virtual const char * GetModulePath() = 0;


	virtual void CloseDB() = 0;


	/**
	* 是否记住密码
	*/
	virtual bool GetLoginParams(vector<sLoginUser>& ) = 0;

	/**
	* 保存登陆选项配置信息
	*/
	virtual bool SaveLoginParams(tstring& strPhoneNum, tstring strPwd, bool bRemPwd, bool bAutoLogin, __int64 sTime) = 0;
	
	/**
	* 保存系统消息
	*/
	virtual bool SaveSysMessage(sSysMsg& sysMsg) = 0;

	/**
	* 加载系统消息
	*/
	virtual bool LoadSysMessage(vector<sSysMsg>& ) = 0;
	/**
	* 获取最后一条系统消息
	*/
	virtual bool GetLastSysMsg(sSysMsg& sysMsg) = 0;
	/**
	* 更新系统消息的处理状态：处理：未处理
	*/
	virtual bool UpdateSysMsgStatus(time_t time, bool bProcessed) = 0;

	/**
	* 缓存本地消息记录
	*/
	virtual bool SaveChatMessage(IM_UID uid, _tChatMessage& tMsg) = 0;

	/**
	* 删除本地消息记录
	*/
	virtual bool RemoveChatMessage(IM_UID uid)=0;

	/**
	* 加载本地消息记录
	*/
	virtual bool LoadChatMessage(IM_UID uid, vector<_tChatMessage>& vecMsgs)=0;
	/**
	* 获取最后一次的聊天记录s
	*/
	virtual bool LoadLastMessage(IM_UID uid, _tChatMessage& tLastMsg)=0;

	/**
	* 缓存本地群消息记录
	*/
	virtual bool SaveGroupChatMsg(const tstring& strRoomID, _tGroupChatMessage& tGroupMsg) = 0;

	/**
	* 删除本地群消息记录
	*/
	virtual bool RemoveGroupChatMessage(const tstring& room_id) = 0;
		
	/**
	* 加载本地群消息记录
	*/
	virtual bool LoadGroupChatMsg(const tstring& strRoomID, vector<_tGroupChatMessage>& tGroupMsg) = 0;

	/**
	* 加载最后一条消息
	*/
	virtual bool LoadLastGroupChatMsg(const tstring& strRoomID, _tGroupChatMessage& groupMsg) = 0;

	/**
	*缓存好友的基本信息
	*/
	virtual bool SaveUserVCard(IM_UID uid,sUserVCard/*_tUserInfo*/& useinfo) = 0;
	virtual bool LoadUserVCard(IM_UID uid,sUserVCard& useinfo) = 0;
	virtual bool LoadContactBaseInfo(IM_UID uid,_tUserInfo& useinfo) = 0;
	

	/**
	* 缓存本地的群组列表
	*/
	virtual bool SaveRecentGroupItem(RecentGroupItem& oneGroup) = 0;

	/**
	* 从本地群组数据库中删除该群组
	*/
	virtual bool RemoveFromRecentGroup(const tstring& room_id) = 0;

	/**
	* 更新本地群组数据库中群头像路径
	*/
	virtual bool UpdateAvatarInRecentGroup(const tstring& room_id, const tstring& avatar_path) = 0;


	/**
	* 加载本地的群组列表
	*/
	virtual bool LoadRecentGroupItem(vector<RecentGroupItem>& recentGroup) = 0;

	/**
	* 批量缓存本地最近联系人列表
	*/
	virtual bool SaveRecentFriendList(RECENTITEM& recent /*vector<RECENTITEM>& recentlist*/) = 0;
	/**
	* 加载本地最近联系人列表
	*/
	virtual bool LoadRecentFriendList(vector<RECENTITEM>& recentlist) = 0;
	/**
	* 删除最近联系人
	*/
	virtual bool RemoveRecentItem(IM_UID  uid) = 0;
	/**
	* 缓存一个最近联系人记录
	*/
	virtual bool AddRecentItem(RECENTITEM& item) = 0;

	virtual bool LoadCallHistoryList(vector<CallHistoryItem>& recentlist) = 0;
	virtual bool RemoveCallHistoryItem(IM_UID uid) = 0;
	virtual bool AddCallHistoryItem(CallHistoryItem& item) = 0;
	virtual bool SaveCallHistoryList(const tstring& strAccount, const EDialIndicate eCallType) = 0;

	virtual bool SaveSendMsgHotKey(int keyMode) = 0;
	virtual int GetSendMsgHotKey() = 0;
	virtual bool IsFirstLogin()=0;
};

class IDBTaskModuleEvent
{

};


/// dll接口
//extern "C"
//{
	/*AFX_EXT_API*/ IDBTaskModule* GetDBTaskModule();
//}