#pragma once

#include <string>
#include <cstdio>
#include <map>
#include <list>

#include "../gloox-1.0.14/src/client.h"
#include "../gloox-1.0.14/src/rosterlistener.h"
#include "../gloox-1.0.14/src/rostermanager.h"
#include "../gloox-1.0.14/src/vcardhandler.h"
#include "../gloox-1.0.14/src/vcardmanager.h"
#include "../gloox-1.0.14/src/vcard.h"

#include "../Include/Global_Def.h"
#include "../Common/UserInfoDef.h"
#include "../include/UsrManagerEvent_i.h"

/**
* 获取用户(自己)Roster信息
* 获取用户(自己)VCard信息
* Tips:一个Client对应一个pVCardManager.
*/

using namespace gloox;
using namespace std;

class CUserInfoManager : public gloox::RosterListener,
						 public gloox::VCardHandler
						 //public CTrayTaskEventBase
						 //public gloox::SubscriptionHandler
{
public:
	CUserInfoManager();
	virtual ~CUserInfoManager();

	void AddEventHandler(IUserManagerEvent* pHandler);
	void RemoveEventHandler(IUserManagerEvent* pHandler);

	/**
	* Get all roster cache.
	*/
	bool GetAllRosters(MapAllRosters& mapRoster);
	
	/**
	* Register thismodule to listen event;
	*/
	void Initialize(Client* pClient);
	
	/**
	* Send get vcard request to xmpp server.
	*/
	void FetchVCardByAccount(tstring& strAccount);

	/**
     * Use this function to store or update your own VCard on the server. Remember to
     * always send a full VCard, not a delta of changes.
	 */
	void StoreMyVCard(sUserVCard& newInfo);

	/*
	* Update state of myself.
	*/
	void SetMyselfState(EUsrState eState);

	/**
	* 获取用户的Roster信息
	*/
	bool GetOneUserRoster(tstring& strAccount, sRosterItemLocal& sOutRoster);

	/**
	* Add one user.
	*/
	void AddOneUser(tstring& strUserAccount, const tstring strMsg = MMPC_EMPTYSTR, const tstring strNick = MMPC_EMPTYSTR);
	
	/*
	* Remove one user.
	*/
	void RemoveUser(tstring& strUsrAccount);

	/**
	* 获取当前状态
	*/
	EUsrState GetState(const tstring& strAccount);

	/**
	* Get batch group member's vcard.
	*/
	void FetchGroupMemVCards(vector<tstring>& vecGroupMembers);

	/**
	* 修改备注
	*/
	void UpdateMemo(const tstring& strAccount, tstring& strNewMemo);

	/**
	* 获取备注名
	*/
	tstring GetMemo(const tstring& strAcount);

	/**
	* 获取自己所在组织机构名和ID
	*/
	bool GetMyOrganization(tstring& strMyOrg, tstring& strOrgID);
	
	/**
	* Get org name by account.
	*/
	tstring GetOrgName(const tstring& strAcount);
	
	//subscriptionHandler:加好友请求结果回调
	//virtual void handleSubscription( const Subscription& subscription ) ;
	virtual void handleItemSubscribed( const JID& jid , const std::tstring& strSendNick=(_T("")));
	virtual void handleItemUnsubscribed( const JID& jid );
	virtual void handleItemUpdated( const JID& jid );


	/**
	* 添加好友成功：gloox hack了这个过程。与xmpp协议不一致。
	*/
	virtual void handleItemAdded( const JID& jid );
	
	/**
	* 好友删除了
	*/
	virtual void handleItemRemoved( const JID& jid );
	
	/**
	* Server下发的所有的My Roster信息。
	*/
	virtual void handleRoster( const Roster& roster );
	virtual void handleRosterError( const IQ& /*iq*/ );

	/**
	* 上报好友Roster的状态信息。注意：账号可能相同，但如果resource不同
	* 也会上报。msg:用户自定义的状态描述语。
	*/
	virtual void handleRosterPresence( const RosterItem& item, const std::tstring& resource,
										Presence::PresenceType presence, const std::tstring& msg );
	
	/**
	* 上报自己的状态信息，此处的resource是自己登陆时候设置的。
	* 如果没有设置resource，server会默认设置一个资源组。
	* msg:用户自定义的状态描述语。
	*/
	virtual void handleSelfPresence( const RosterItem& item, const std::tstring& resource,
		Presence::PresenceType presence, const std::tstring& msg );
	/**
	* 有人要加我: 
	* 1:return true(同意)==>发送subscribed jid给server。
	    并调用ackSubscriptionRequest(jid, true);===>handleItemAdded(好友添加成功)
	* 2:return false(拒绝)=>发送unsubscribed jid给server,我拒绝对方，
	    并调用ackSubscriptionRequest(jid, false);
	*/
	virtual bool handleSubscriptionRequest( const JID& jid, const std::tstring& msg ,const std::tstring& strSendNick=(_T("")));
	virtual bool handleUnsubscriptionRequest( const JID& jid, const std::tstring& msg);
	virtual void handleNonrosterPresence( const Presence& presence );

	/**
	* Callback of fetchVCard(...);
	*/
	 virtual void handleVCard( const JID& jid, const VCard* vcard );
     
	 /**
	 * Callback of storeVCard(...); store/update Successfully if se == StanzaErrorUndefined.
	 */
	 virtual void handleVCardResult( VCardContext context, const JID& jid,
                                     StanzaError se = StanzaErrorUndefined  );

	/**
	* 判断是不是我的好友
	*/
	bool IsMyFriend(const tstring& strAccount);
	bool IsMySelf(const tstring& strAccount);

	/**
	* 获取好友头像，昵称
	*/
	bool GetUserVCard(const tstring& strAccount, sUserVCard& oneVCard);

protected:
	void UpdateCacheAfterFriendRemoved(tstring& strAccount);
	void FetchvcardByJid( const JID& jid );

private:
	VCardManager* m_pVCardMgr;
	//缓存：好友列表以及状态信息
	MapAllRosters m_mapAccount2Rosters;
	CRITICAL_SECTION m_csRosters;

	//缓存：自己和好友的VCard信息
	MapAccount2VCards m_mapUser2VCard;
	CRITICAL_SECTION m_csVCards;
	CRITICAL_SECTION m_csListener;
	//Handlers
	list<IUserManagerEvent*> m_listHandlers;
};