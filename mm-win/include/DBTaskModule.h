/*
@file:		DBTaskModule.h
@purpose:	�������ݿ�ӿڣ������ṩ������Ϣ�����õı��ش�ȡ
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
	virtual void AddEventHandler(IDBTaskModuleEvent* pEventHandler) = 0;	// ����¼�������
	virtual void Release() = 0;

	/// ��ģ���ʵ�ʹ��ܽӿ�
	//SaveRecent(vector<uin>) = 0;

	virtual bool SetDBPath(IM_UID uid) = 0;

	virtual void Initialize() = 0;

	virtual const char * GetModulePath() = 0;


	virtual void CloseDB() = 0;


	/**
	* �Ƿ��ס����
	*/
	virtual bool GetLoginParams(vector<sLoginUser>& ) = 0;

	/**
	* �����½ѡ��������Ϣ
	*/
	virtual bool SaveLoginParams(tstring& strPhoneNum, tstring strPwd, bool bRemPwd, bool bAutoLogin, __int64 sTime) = 0;
	
	/**
	* ����ϵͳ��Ϣ
	*/
	virtual bool SaveSysMessage(sSysMsg& sysMsg) = 0;

	/**
	* ����ϵͳ��Ϣ
	*/
	virtual bool LoadSysMessage(vector<sSysMsg>& ) = 0;
	/**
	* ��ȡ���һ��ϵͳ��Ϣ
	*/
	virtual bool GetLastSysMsg(sSysMsg& sysMsg) = 0;
	/**
	* ����ϵͳ��Ϣ�Ĵ���״̬������δ����
	*/
	virtual bool UpdateSysMsgStatus(time_t time, bool bProcessed) = 0;

	/**
	* ���汾����Ϣ��¼
	*/
	virtual bool SaveChatMessage(IM_UID uid, _tChatMessage& tMsg) = 0;

	/**
	* ɾ��������Ϣ��¼
	*/
	virtual bool RemoveChatMessage(IM_UID uid)=0;

	/**
	* ���ر�����Ϣ��¼
	*/
	virtual bool LoadChatMessage(IM_UID uid, vector<_tChatMessage>& vecMsgs)=0;
	/**
	* ��ȡ���һ�ε������¼s
	*/
	virtual bool LoadLastMessage(IM_UID uid, _tChatMessage& tLastMsg)=0;

	/**
	* ���汾��Ⱥ��Ϣ��¼
	*/
	virtual bool SaveGroupChatMsg(const tstring& strRoomID, _tGroupChatMessage& tGroupMsg) = 0;

	/**
	* ɾ������Ⱥ��Ϣ��¼
	*/
	virtual bool RemoveGroupChatMessage(const tstring& room_id) = 0;
		
	/**
	* ���ر���Ⱥ��Ϣ��¼
	*/
	virtual bool LoadGroupChatMsg(const tstring& strRoomID, vector<_tGroupChatMessage>& tGroupMsg) = 0;

	/**
	* �������һ����Ϣ
	*/
	virtual bool LoadLastGroupChatMsg(const tstring& strRoomID, _tGroupChatMessage& groupMsg) = 0;

	/**
	*������ѵĻ�����Ϣ
	*/
	virtual bool SaveUserVCard(IM_UID uid,sUserVCard/*_tUserInfo*/& useinfo) = 0;
	virtual bool LoadUserVCard(IM_UID uid,sUserVCard& useinfo) = 0;
	virtual bool LoadContactBaseInfo(IM_UID uid,_tUserInfo& useinfo) = 0;
	

	/**
	* ���汾�ص�Ⱥ���б�
	*/
	virtual bool SaveRecentGroupItem(RecentGroupItem& oneGroup) = 0;

	/**
	* �ӱ���Ⱥ�����ݿ���ɾ����Ⱥ��
	*/
	virtual bool RemoveFromRecentGroup(const tstring& room_id) = 0;

	/**
	* ���±���Ⱥ�����ݿ���Ⱥͷ��·��
	*/
	virtual bool UpdateAvatarInRecentGroup(const tstring& room_id, const tstring& avatar_path) = 0;


	/**
	* ���ر��ص�Ⱥ���б�
	*/
	virtual bool LoadRecentGroupItem(vector<RecentGroupItem>& recentGroup) = 0;

	/**
	* �������汾�������ϵ���б�
	*/
	virtual bool SaveRecentFriendList(RECENTITEM& recent /*vector<RECENTITEM>& recentlist*/) = 0;
	/**
	* ���ر��������ϵ���б�
	*/
	virtual bool LoadRecentFriendList(vector<RECENTITEM>& recentlist) = 0;
	/**
	* ɾ�������ϵ��
	*/
	virtual bool RemoveRecentItem(IM_UID  uid) = 0;
	/**
	* ����һ�������ϵ�˼�¼
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


/// dll�ӿ�
//extern "C"
//{
	/*AFX_EXT_API*/ IDBTaskModule* GetDBTaskModule();
//}