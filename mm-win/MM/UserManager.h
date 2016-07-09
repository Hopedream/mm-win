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
* ��ȡ�û�(�Լ�)Roster��Ϣ
* ��ȡ�û�(�Լ�)VCard��Ϣ
* Tips:һ��Client��Ӧһ��pVCardManager.
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
	* ��ȡ�û���Roster��Ϣ
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
	* ��ȡ��ǰ״̬
	*/
	EUsrState GetState(const tstring& strAccount);

	/**
	* Get batch group member's vcard.
	*/
	void FetchGroupMemVCards(vector<tstring>& vecGroupMembers);

	/**
	* �޸ı�ע
	*/
	void UpdateMemo(const tstring& strAccount, tstring& strNewMemo);

	/**
	* ��ȡ��ע��
	*/
	tstring GetMemo(const tstring& strAcount);

	/**
	* ��ȡ�Լ�������֯��������ID
	*/
	bool GetMyOrganization(tstring& strMyOrg, tstring& strOrgID);
	
	/**
	* Get org name by account.
	*/
	tstring GetOrgName(const tstring& strAcount);
	
	//subscriptionHandler:�Ӻ����������ص�
	//virtual void handleSubscription( const Subscription& subscription ) ;
	virtual void handleItemSubscribed( const JID& jid , const std::tstring& strSendNick=(_T("")));
	virtual void handleItemUnsubscribed( const JID& jid );
	virtual void handleItemUpdated( const JID& jid );


	/**
	* ��Ӻ��ѳɹ���gloox hack��������̡���xmppЭ�鲻һ�¡�
	*/
	virtual void handleItemAdded( const JID& jid );
	
	/**
	* ����ɾ����
	*/
	virtual void handleItemRemoved( const JID& jid );
	
	/**
	* Server�·������е�My Roster��Ϣ��
	*/
	virtual void handleRoster( const Roster& roster );
	virtual void handleRosterError( const IQ& /*iq*/ );

	/**
	* �ϱ�����Roster��״̬��Ϣ��ע�⣺�˺ſ�����ͬ�������resource��ͬ
	* Ҳ���ϱ���msg:�û��Զ����״̬�����
	*/
	virtual void handleRosterPresence( const RosterItem& item, const std::tstring& resource,
										Presence::PresenceType presence, const std::tstring& msg );
	
	/**
	* �ϱ��Լ���״̬��Ϣ���˴���resource���Լ���½ʱ�����õġ�
	* ���û������resource��server��Ĭ������һ����Դ�顣
	* msg:�û��Զ����״̬�����
	*/
	virtual void handleSelfPresence( const RosterItem& item, const std::tstring& resource,
		Presence::PresenceType presence, const std::tstring& msg );
	/**
	* ����Ҫ����: 
	* 1:return true(ͬ��)==>����subscribed jid��server��
	    ������ackSubscriptionRequest(jid, true);===>handleItemAdded(������ӳɹ�)
	* 2:return false(�ܾ�)=>����unsubscribed jid��server,�Ҿܾ��Է���
	    ������ackSubscriptionRequest(jid, false);
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
	* �ж��ǲ����ҵĺ���
	*/
	bool IsMyFriend(const tstring& strAccount);
	bool IsMySelf(const tstring& strAccount);

	/**
	* ��ȡ����ͷ���ǳ�
	*/
	bool GetUserVCard(const tstring& strAccount, sUserVCard& oneVCard);

protected:
	void UpdateCacheAfterFriendRemoved(tstring& strAccount);
	void FetchvcardByJid( const JID& jid );

private:
	VCardManager* m_pVCardMgr;
	//���棺�����б��Լ�״̬��Ϣ
	MapAllRosters m_mapAccount2Rosters;
	CRITICAL_SECTION m_csRosters;

	//���棺�Լ��ͺ��ѵ�VCard��Ϣ
	MapAccount2VCards m_mapUser2VCard;
	CRITICAL_SECTION m_csVCards;
	CRITICAL_SECTION m_csListener;
	//Handlers
	list<IUserManagerEvent*> m_listHandlers;
};