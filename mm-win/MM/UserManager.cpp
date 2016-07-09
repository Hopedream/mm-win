#include "UserManager.h"
#include "Controller.h"
#include "..\include\Global_Def.h"
#include "..\gloox-1.0.14\src\subscription.h"
#include "..\include\TrayTaskModule.h"
#include "StringHelper.h"
#include "FileHelper.h"
#include <sstream>
#include "MagicKHelper.h"
#include "MsgBox.h"
#include "StringHelper.h"
#include "Utility.h"
#include "UIDefine.h"

CUserInfoManager::CUserInfoManager()
{
	m_pVCardMgr = NULL;
	::InitializeCriticalSection(&m_csRosters);
	::InitializeCriticalSection(&m_csVCards);
	::InitializeCriticalSection(&m_csListener);
}

CUserInfoManager::~CUserInfoManager()
{
	//When cleaning up, delete your VCardManager instance 
	//@b before deleting the Client/ClientBase instance.
	if (NULL != m_pVCardMgr)
	{
		delete m_pVCardMgr;
		m_pVCardMgr = NULL;
	}

	::DeleteCriticalSection(&m_csRosters);
	::DeleteCriticalSection(&m_csVCards);
	::DeleteCriticalSection(&m_csListener);
}

void CUserInfoManager::AddEventHandler(IUserManagerEvent* pHandler)
{
	if (NULL != pHandler)
	{
		::EnterCriticalSection(&m_csListener);
		bool bFound = false;
		for (list<IUserManagerEvent*>::iterator it = m_listHandlers.begin();
			it != m_listHandlers.end(); ++it)
		{
			IUserManagerEvent* pLocal = *it;
			if (pLocal == pHandler)
			{
				bFound = true;
				break;;
			}
		}

		if (!bFound)
		{
			m_listHandlers.push_back(pHandler);
		}
		::LeaveCriticalSection(&m_csListener);
	}
}

void CUserInfoManager::Initialize(Client* pClient)
{
	//add to tray task to receive add friend notify.
	//GetTrayTaskModule()->AddEventHandler((ITrayTaskModuleEvent*)(this));

	//
	if (NULL != pClient)
	{
		//pClient->registerSubscriptionHandler(this);
		pClient->rosterManager()->registerRosterListener( this, false);
		m_pVCardMgr = new VCardManager(pClient);
		
		//fetch myself vcard.
		FetchvcardByJid(pClient->jid());
	}
}

void CUserInfoManager::FetchVCardByAccount(tstring& strAccount)
{
	if (strAccount.empty())
	{
		return;
	}

	tstring strTmp = strAccount;
	int nIdex = strTmp.find('@', 0);
	if (nIdex == -1)
	{
		strTmp += MMPC_DOMAIN;
	}

	if (strTmp.length() == tstring(MMPC_DOMAIN).length()+11 ||
		strTmp.length() == tstring(_T("@test-d")).length()+11)//11:�绰����ĳ��ȣ�
	{
		JID jid(strTmp);
		FetchvcardByJid(jid);
	}
}

void CUserInfoManager::FetchvcardByJid( const JID& jid )
{
	if (NULL != m_pVCardMgr)
	{
		m_pVCardMgr->fetchVCard( jid, this );
	}
}

void CUserInfoManager::StoreMyVCard(sUserVCard& newInfo)
{
	if (NULL != m_pVCardMgr)
	{
		VCard* v = new VCard();
		v->setFormattedname( newInfo.strAccount);
		v->setNickname(CChineseCode::EncodeUTF8(newInfo.strUserNickname));
		v->addAddress("","", CChineseCode::EncodeUTF8(newInfo.strAddr), "","","","", VCard::AddressType::AddrTypeWork);
		v->addEmail(newInfo.strEmail, VCard::AddressType::AddrTypeWork);
		v->setPhoto(newInfo.strPhotoType, newInfo.strPhotoBinary);
		v->addTelephone(newInfo.strPhone, VCard::AddressType::AddrTypeWork);   //�ֻ�
		v->setNote(CChineseCode::EncodeUTF8(newInfo.strSignature));
		v->setGeo( CChineseCode::EncodeUTF8(newInfo.strGender),newInfo.strAge.length() == 0? " ":newInfo.strAge);	
		v->setOrganization(CChineseCode::EncodeUTF8(newInfo.strOrganization), StringList());
		v->addTelephone(newInfo.strTelephone, VCard::AddressType::AddrTypeHome);   //�绰
		v->addEmail(CChineseCode::EncodeUTF8(newInfo.strVocation), VCard::AddressType::AddrTypeHome);//��ҵ��todo��û��Ӧ�ֶΣ���ʱ������email�ֶδ���
		m_pVCardMgr->storeVCard( v, this );
		//tstring strXml = v->tag()->xml();
	}
}

void CUserInfoManager::handleItemSubscribed( const JID& jid, const std::tstring& strSendNick )
{
	::EnterCriticalSection(&m_csListener);
	for (list<IUserManagerEvent*>::iterator it = m_listHandlers.begin(); 
		it != m_listHandlers.end(); ++it)
	{
		IUserManagerEvent* pHandler = (IUserManagerEvent*)(*it);
		if (NULL != pHandler)
		{
			pHandler->OnRecvAcceptAddFriendReq(tstring(jid.bare().c_str()),CChineseCode::DecodeUTF8(strSendNick));
		}
	}
	::LeaveCriticalSection(&m_csListener);

	////printf( "subscription: %s\n", jid.bare().c_str() );
	//return false;
}

void CUserInfoManager::handleItemAdded( const JID& jid )
{
	tstring strFromAccount = jid.bare();

	::EnterCriticalSection(&m_csRosters);
	MapAllRosters::iterator itFind = m_mapAccount2Rosters.find(strFromAccount);
	if (itFind == m_mapAccount2Rosters.end())
	{
		RosterManager* pRosterMgr = CController::Instance()->GetXmppClient()->rosterManager();
		if (NULL != pRosterMgr)
		{
			RosterItem* pNewFriend = pRosterMgr->getRosterItem(jid);
			if (NULL != pNewFriend)
			{
				sRosterItemLocal oneRoster;
				oneRoster.strAccount = strFromAccount;
				oneRoster.strMemo = pNewFriend->name() == ExtractPhoneNo(strFromAccount) ? "" : CChineseCode::DecodeUTF8(pNewFriend->name());
				
				StringList g = pNewFriend->groups();
				StringList::const_iterator it_g = g.begin();
				for( ; it_g != g.end(); ++it_g )
				{
					tstring str = (*it_g).c_str();
					if (!str.empty())
					{
						int nIndex = str.find('|');
						int nLength = str.length();
						if (nIndex>0 && nLength>nIndex+1)
						{
							oneRoster.strOrgID = str.substr(0, nIndex);
							oneRoster.strOrgnization = CChineseCode::DecodeUTF8(str.substr(nIndex+1,nLength-nIndex-1));
							break;
						}
						break;
					}
				}
				
				if (oneRoster.strOrgnization.empty())
				{
					oneRoster.strOrgnization = ORGANIZATION_MYFRIEND;
					oneRoster.strOrgID = DEF_ORGID_MYFRIEND;
				}
				//for test.
				//oneRoster.strOrgnization = oneRoster.strNickname;

				RosterItem::ResourceMap::const_iterator rit = pNewFriend->resources().begin();
				for( ; rit != pNewFriend->resources().end(); ++rit )
				{
					tstring str = (*rit).first.c_str() ;
					oneRoster.ListResources.push_back(str);
				}

				m_mapAccount2Rosters.insert(make_pair(strFromAccount, oneRoster));

				//
				::EnterCriticalSection(&m_csListener);
				for (list<IUserManagerEvent*>::iterator it = m_listHandlers.begin(); 
					it != m_listHandlers.end(); ++it)
				{
					IUserManagerEvent* pHandler = (IUserManagerEvent*)(*it);
					if (NULL != pHandler)
					{
						pHandler->OnAddFriendSucc(strFromAccount);
					}
				}
				::LeaveCriticalSection(&m_csListener);

				//��ȡ����º��ѵ�VCard��Ϣ
				FetchvcardByJid(jid);
			}
		}
	}
	::LeaveCriticalSection(&m_csRosters);
}

//����ȡ����������״̬��Ϣ��
void CUserInfoManager::handleItemUnsubscribed( const JID& jid )
{
	tstring str = jid.bare();
	//printf( "unsubscribed %s\n", jid.bare().c_str() );
}

//����ɾ���ˣ�1)����ɾ�����ѳɹ�; 2)��Ӻ��Ѻ󣬺��Ѿܾ��ˡ�3)����ֱ������
void CUserInfoManager::handleItemRemoved( const JID& jid )
{
	tstring strAccount = jid.bare();
	::EnterCriticalSection(&m_csRosters);
	MapAllRosters::iterator itFind = m_mapAccount2Rosters.find(strAccount);
	if (itFind != m_mapAccount2Rosters.end())
	{
		//�����Լ���Roster������Ϣ��
		::EnterCriticalSection(&m_csListener);
		for (list<IUserManagerEvent*>::iterator it = m_listHandlers.begin(); 
			it != m_listHandlers.end(); ++it)
		{
			IUserManagerEvent* pHandler = (IUserManagerEvent*)(*it);
			if (NULL != pHandler)
			{
				pHandler->OnFriendRemoved(strAccount,false);
			}
		
			::LeaveCriticalSection(&m_csListener);
		}
	}
	::LeaveCriticalSection(&m_csRosters);

	//
	UpdateCacheAfterFriendRemoved(strAccount);
}

void CUserInfoManager::handleItemUpdated( const JID& jid )
{
	tstring str = jid.bare();
	//printf( "updated %s\n", jid.bare().c_str() );
}

void CUserInfoManager::handleRoster( const Roster& roster )
{
	::EnterCriticalSection(&m_csRosters);
	//printf( "roster arriving\nitems:\n" );
	if (roster.size() > 0)
	{
		m_mapAccount2Rosters.clear();
	}


	Roster::const_iterator it = roster.begin();
	for( ; it != roster.end(); ++it )
	{
		MapAllRosters::iterator itChild = m_mapAccount2Rosters.find(it->first);

		if (itChild == m_mapAccount2Rosters.end())
		{
			sRosterItemLocal oneRoster;
			oneRoster.strAccount = it->first;
			oneRoster.strMemo=(_T(""));//ȡ���˱�ע����//(*it).second->name() == ExtractPhoneNo(oneRoster.strAccount) ? "" : CChineseCode::DecodeUTF8((*it).second->name());
			//oneRoster.eStatus = it->second->online()? enmState_Online : enmState_Offline;

			// 		SubscriptionType type = (*it).second->subscription();
			// 		//printf( "jid: %s, name: %s, subscription: %d\n",
			// 			(*it).second->jidJID().full().c_str(), (*it).second->name().c_str(),
			// 			(*it).second->subscription() );
			
			//MMԼ��һ����ֻ������һ������
			StringList g = (*it).second->groups();
			StringList::const_iterator it_g = g.begin();
			for( ; it_g != g.end(); ++it_g )
			{
				tstring str = CChineseCode::DecodeUTF8((*it_g));;
				if (!str.empty())
				{
					int nIndex = str.find('|');
					int nLength = str.length();
					if (nIndex>0 && nLength>nIndex+1)
					{
						oneRoster.strOrgID = str.substr(0, nIndex);
						oneRoster.strOrgnization = str.substr(nIndex+1,nLength-nIndex-1);
						break;
					}
				}
			}
			if (oneRoster.strOrgnization.empty())
			{
				oneRoster.strOrgnization = ORGANIZATION_MYFRIEND;
				oneRoster.strOrgID = DEF_ORGID_MYFRIEND;
			}
			//for test.
			//oneRoster.strOrgnization = oneRoster.strNickname;

			RosterItem::ResourceMap::const_iterator rit = (*it).second->resources().begin();
			for( ; rit != (*it).second->resources().end(); ++rit )
			{
				tstring str = (*rit).first.c_str() ;
				oneRoster.ListResources.push_back(str);
				//printf( "resource: %s\n", (*rit).first.c_str() );

			}

			//cache all rosters.
			m_mapAccount2Rosters.insert(make_pair(oneRoster.strAccount, oneRoster));
		}
	}

	//notify to observers.
	//if (m_mapAccount2Rosters.size()>0)
	{
		::EnterCriticalSection(&m_csListener);
		for (list<IUserManagerEvent*>::iterator it = m_listHandlers.begin(); 
			it != m_listHandlers.end(); ++it)
		{
			IUserManagerEvent* pHandler = (IUserManagerEvent*)(*it);
			if (NULL != pHandler)
			{
				pHandler->OnRosterAvalible(m_mapAccount2Rosters);
			}
		}
		::LeaveCriticalSection(&m_csListener);
	}

	::LeaveCriticalSection(&m_csRosters);
}

bool CUserInfoManager::GetMyOrganization(tstring& strMyOrg, tstring& strOrgID)
{
	tstring str = CController::Instance()->GetXmppClient()->jid().bare();
	::EnterCriticalSection(&m_csRosters);
	MapAllRosters::iterator itFind = m_mapAccount2Rosters.find(CController::Instance()->GetXmppClient()->jid().bare());
	if (itFind != m_mapAccount2Rosters.end())
	{
		strOrgID = itFind->second.strOrgID;
		strMyOrg = itFind->second.strOrgnization;
	}
	::LeaveCriticalSection(&m_csRosters);

	return (!strMyOrg.empty() && !strOrgID.empty());
}

tstring CUserInfoManager::GetMemo(const tstring& strAcount)
{
	tstring strMemo;
	::EnterCriticalSection(&m_csRosters);
	MapAllRosters::iterator itFind = m_mapAccount2Rosters.find(strAcount);
	if (itFind != m_mapAccount2Rosters.end())
	{
		strMemo = itFind->second.strMemo;
	}
	::LeaveCriticalSection(&m_csRosters);
	strMemo.clear();  //lwt
	return strMemo;
}

tstring CUserInfoManager::GetOrgName(const tstring& strAcount)
{
	tstring strOrgnization;
	::EnterCriticalSection(&m_csRosters);
	MapAllRosters::iterator itFind = m_mapAccount2Rosters.find(strAcount);
	if (itFind != m_mapAccount2Rosters.end())
	{
		strOrgnization = itFind->second.strOrgnization;
	}
	::LeaveCriticalSection(&m_csRosters);

	return strOrgnization;
}

void CUserInfoManager::handleRosterError( const IQ& /*iq*/ )
{
	////printf( "a roster-related error occured\n" );
}

void CUserInfoManager::handleRosterPresence( const RosterItem& item, const std::tstring& resource,
	Presence::PresenceType presence, const std::tstring& msg )
{
	tstring strAccount = item.jidJID().bare().c_str();
	if (strAccount.empty())
		return;
	tstring strRes = resource.c_str();
	tstring strmsg = msg;
	Presence::PresenceType type = presence;
	EUsrState eState=enmState_Offline;//lee:�������ʼ�������ֵ�Ǹ��Ƿ�ֵ����ô�������OnuserStateUdpate�����л���ֺ�������call stack�Ĵ���

	switch (type)
	{
	case Presence::PresenceType::Available:
		eState = enmState_Online;
		break;
	case Presence::PresenceType::Unavailable:
		eState = enmState_Offline;
		break;
	case Presence::PresenceType::Away:
		eState = enmState_Busy;
		break;
	default:
		break;
	}
	
	//���»���.
	::EnterCriticalSection(&m_csRosters);
	MapAllRosters::iterator itFind = m_mapAccount2Rosters.find(strAccount);
	if (itFind != m_mapAccount2Rosters.end())
	{
		itFind->second.eStatus = eState;
		if (eState == enmState_Busy)
		{
			itFind->second.strBusyTips = strmsg;
		}
	}
	::LeaveCriticalSection(&m_csRosters);

	//֪ͨUI����
	::EnterCriticalSection(&m_csListener);
	for (list<IUserManagerEvent*>::iterator it = m_listHandlers.begin(); 
		it != m_listHandlers.end(); ++it)
	{
		IUserManagerEvent* pHandler = (IUserManagerEvent*)(*it);
		if (NULL != pHandler)
		{
			pHandler->OnUserStateUpdated(strAccount, eState, strRes, strmsg);
		}
	}
	::LeaveCriticalSection(&m_csListener);

	////printf( "presence received: %s/%s -- %d\n", item.jidJID().bare().c_str(), resource.c_str(), presence );
}

void CUserInfoManager::handleSelfPresence( const RosterItem& item, const std::tstring& resource,
	Presence::PresenceType presence, const std::tstring& msg )
{
	tstring strAccount = item.jidJID().bare().c_str();
	tstring strRes = resource.c_str();
	tstring strMsg = msg;
	Presence::PresenceType type = presence;

	EUsrState eState=enmState_Offline;

	switch (type)
	{
	case Presence::PresenceType::Available:
		eState = enmState_Online;
		break;
	case Presence::PresenceType::Unavailable:
		eState = enmState_Offline;
		break;
	case Presence::PresenceType::Away:
		eState = enmState_Busy;
		break;
	default:
		break;
	}

	::EnterCriticalSection(&m_csListener);
	for (list<IUserManagerEvent*>::iterator it = m_listHandlers.begin(); 
		it != m_listHandlers.end(); ++it)
	{
		IUserManagerEvent* pHandler = (IUserManagerEvent*)(*it);
		if (NULL != pHandler)
		{
			pHandler->OnMySelfStateUpdate(strAccount, eState, strRes, strMsg);
		}
	}
	::LeaveCriticalSection(&m_csListener);


	//////printf( "self presence received: %s/%s -- %d\n", item.jidJID().bare().c_str(), resource.c_str(), presence );
}

bool CUserInfoManager::handleSubscriptionRequest( const JID& jid, const std::tstring& msg ,const std::tstring& strSendNick /*= EmptyString*/)
{
	::EnterCriticalSection(&m_csListener);
	for (list<IUserManagerEvent*>::iterator it = m_listHandlers.begin(); 
		it != m_listHandlers.end(); ++it)
	{
		IUserManagerEvent* pHandler = (IUserManagerEvent*)(*it);
		if (NULL != pHandler)
		{
			pHandler->OnRecvAddFriendReq(tstring(jid.bare().c_str()), CChineseCode::DecodeUTF8(msg), CChineseCode::DecodeUTF8(strSendNick));
		}
	}
	::LeaveCriticalSection(&m_csListener);

	////printf( "subscription: %s\n", jid.bare().c_str() );
	return false;
}

//�Է�ɾ���ң��Ҳ�������ֱ��return true,����һ��unscribed jid ��server,��server��jid���ҵĺ����б���ɾ��
bool CUserInfoManager::handleUnsubscriptionRequest( const JID& jid, const std::tstring& /*msg*/ )
{
	//printf( "unsubscription: %s\n", jid.bare().c_str() );
	return true;
}

void CUserInfoManager::handleNonrosterPresence( const Presence& presence )
{
	//printf( "received presence from entity not in the roster: %s\n", presence.from().bare().c_str() );
}


void CUserInfoManager::handleVCard( const JID& jid, const VCard *v )
{
	if( !v )
	{
		//printf( "empty vcard!\n" );
		return;
	}
	tstring strAccount = jid.bare().c_str();
	tstring strxml = v->tag()->xml().c_str();
	////printf( "received vcard for %s: %s, %d\n", jid.full().c_str(), v->tag()->xml().c_str(), m_count );
	
	sUserVCard oneCard;
	oneCard.strAccount = strAccount;
	oneCard.strUserNickname = v->nickname().empty()? jid.username() : CChineseCode::DecodeUTF8(v->nickname());

	oneCard.strPhotoBinary = v->photo().binval;
	oneCard.strPhotoType = v->photo().type;
	oneCard.strUrl = v->photo().extval;	
	oneCard.strGender = CChineseCode::DecodeUTF8(v->geo().latitude);
	oneCard.strAge = v->geo().longitude;	
	oneCard.strOrganization = CChineseCode::DecodeUTF8( v->org().name);
	oneCard.strSignature = CChineseCode::DecodeUTF8(v->note());

	//get one address .
	VCard::AddressList::const_iterator it = v->addresses().begin();
	for( ; it != v->addresses().end(); ++it )
	{
		oneCard.strAddr = CChineseCode::DecodeUTF8((*it).street.c_str());//get one is enough.
		//printf( "address: %s\n", (*it).street.c_str() );
		break;
	}

	//get one email.
	VCard::EmailList::const_iterator itEmail = v->emailAddresses().begin();
	if (itEmail != v->emailAddresses().end())
	{
		if (itEmail->home)
			oneCard.strVocation = CChineseCode::DecodeUTF8(itEmail->userid);
		if (itEmail->work)
			oneCard.strEmail = itEmail->userid;
	}

	for (VCard::TelephoneList::const_iterator iter = v->telephone().begin(); iter != v->telephone().end(); ++iter)
	{
		if (iter->home)
			oneCard.strTelephone = iter->number;
		if (iter->work)
			oneCard.strPhone = iter->number;
		//break;
	}
	//���û�е绰����ֱ�����õ�½���ֻ��š�
	if (oneCard.strPhone.empty())
	{
		oneCard.strPhone = jid.username();
	}

	//����base64����ͷ����Ϣ�����浽���ء�������Կ��Ƿŵ������ģ��ȥ��===to be refactored.
	if (!oneCard.strPhotoBinary.empty())
	{
		DuiLib::CDuiString strAvatar  = CFileHelper::GetHeadCacheDir();
		strAvatar += strAccount.c_str();
		//strAvatar += getTmpFileId().c_str();
		if(CMagicKHelper::Instance()->GeneratePng(oneCard.strPhotoBinary, tstring(strAvatar.GetData())))
		{
			oneCard.strPhotoPath = tstring(strAvatar + _T(".png"));
			oneCard.strGrayAvatar = CMagicKHelper::Instance()->GrayImage(oneCard.strPhotoPath);
		}
	}

	::EnterCriticalSection(&m_csRosters);
	MapAllRosters::iterator itFind = m_mapAccount2Rosters.find(strAccount);
	if (itFind != m_mapAccount2Rosters.end())
	{
		oneCard.strRemark = itFind->second.strMemo;
	}
	::LeaveCriticalSection(&m_csRosters);
	

	//cache and update:�����Ǻ��ѻ���İ���ˣ������档
	::EnterCriticalSection(&m_csVCards);
	MapAccount2VCards::iterator itFindCard = m_mapUser2VCard.find(strAccount);
	if (itFindCard != m_mapUser2VCard.end())
	{
		sUserVCard& user = itFindCard->second;
		user.strAddr = oneCard.strAddr;
		user.strEmail = oneCard.strEmail;
		user.strPhone = oneCard.strPhone;
		user.strPhotoBinary = oneCard.strPhotoBinary;
		user.strPhotoType = oneCard.strPhotoType;
		user.strUrl = oneCard.strUrl;
		user.strAccount = oneCard.strAccount;
		user.strUserNickname = oneCard.strUserNickname;
		user.strPhotoPath = oneCard.strPhotoPath;
	}
	else
	{
		m_mapUser2VCard.insert(make_pair(strAccount, oneCard));
	}
	::LeaveCriticalSection(&m_csVCards);


	//�����Ǹ��ϲ��ṩ�Ĳ��ҹ���
	if (IsMyFriend(strAccount) || IsMySelf(strAccount))
	{
		//Notify to observers.
		::EnterCriticalSection(&m_csListener);
		for (list<IUserManagerEvent*>::iterator it = m_listHandlers.begin(); 
			it != m_listHandlers.end(); ++it)
		{
			IUserManagerEvent* pHandler = (IUserManagerEvent*)(*it);
			if (NULL != pHandler)
			{
				if (IsMySelf(strAccount))
				{
					pHandler->OnMySelfCardInfo(oneCard);
				}
				else
				{
					pHandler->OnVCardReturned(strAccount, oneCard);
				}
			}
		}
		::LeaveCriticalSection(&m_csListener);
	}
	else//İ������Ϣ
	{
		//Notify to observers.
		::EnterCriticalSection(&m_csListener);
		try{
			for (list<IUserManagerEvent*>::iterator it = m_listHandlers.begin(); 
				it != m_listHandlers.end(); ++it)
			{
				IUserManagerEvent* pHandler = (IUserManagerEvent*)(*it);
				if (NULL != pHandler)
				{
					pHandler->OnStrangerVCard(strAccount, oneCard);				
				}
			}
		}
		catch(...)
		{
			return;
		}
		::LeaveCriticalSection(&m_csListener);
	}
}

void CUserInfoManager::handleVCardResult( VCardContext context, const JID& jid,
	StanzaError se /*= StanzaErrorUndefined*/  )
{
	//printf( "vcard result: context: %d, jid: %s, error: %d\n", context, jid.bare().c_str(), se );

	if (VCardContext::StoreVCard == context)
	{
		::EnterCriticalSection(&m_csListener);
		for (list<IUserManagerEvent*>::iterator it = m_listHandlers.begin(); 
			it != m_listHandlers.end(); ++it)
		{
			IUserManagerEvent* pHandler = (IUserManagerEvent*)(*it);
			if (NULL != pHandler)
			{
				tstring strAccount = jid.bare().c_str();
				pHandler->OnStoreVCardResult(strAccount, StanzaErrorUndefined == se);
			}
		}
		::LeaveCriticalSection(&m_csListener);
	}
}

bool CUserInfoManager::GetUserVCard(const tstring& strAccount, sUserVCard& oneVCard)
{
	//����VCard
	bool bFound = false;
	::EnterCriticalSection(&m_csVCards);
	MapAccount2VCards::iterator itFind2 = m_mapUser2VCard.find(strAccount);
	if (itFind2 != m_mapUser2VCard.end())
	{
		oneVCard = itFind2->second;
		bFound = true;
	}
	::LeaveCriticalSection(&m_csVCards);

	return bFound;
}

bool CUserInfoManager::GetAllRosters(MapAllRosters& mapRoster)
{
	::EnterCriticalSection(&m_csRosters);
	mapRoster = m_mapAccount2Rosters;
	::LeaveCriticalSection(&m_csRosters);
	return mapRoster.size()>0;
}

void CUserInfoManager::SetMyselfState(EUsrState eState)
{
	if (enmState_Online == eState)
	{
		CController::Instance()->GetXmppClient()->setPresence(Presence::PresenceType::Available, 100, "");
	}
	else if (enmState_Offline == eState)
	{
		CController::Instance()->GetXmppClient()->setPresence(Presence::PresenceType::Unavailable, 100, "");
	}
	else if (enmState_Busy == eState)
	{
		CController::Instance()->GetXmppClient()->setPresence(Presence::PresenceType::Away, 100, "");
	}
}

bool CUserInfoManager::GetOneUserRoster(tstring& strAccount, sRosterItemLocal& sOutRoster)
{
	bool bSucc = false;
	::EnterCriticalSection(&m_csRosters);
	MapAllRosters::iterator itFind = m_mapAccount2Rosters.find(strAccount);
	if (itFind != m_mapAccount2Rosters.end())
	{
		sOutRoster = itFind->second;
		bSucc = true;
	}
	::LeaveCriticalSection(&m_csRosters);

	return bSucc;
}

void CUserInfoManager::AddOneUser(tstring& strUserAccount, const tstring strMsg/* = MMPC_EMPTYSTR*/, const tstring strNick /*= MMPC_EMPTYSTR*/)
{
	if (strUserAccount.empty())
	{
		return;
	}

	int nIdex = strUserAccount.find('@', 0);
	if (nIdex == -1)
	{
		strUserAccount += MMPC_DOMAIN;//tstring("@test-d/mmpc");
	}
	RosterManager* pRosterMgr = CController::Instance()->GetXmppClient()->rosterManager();
	if (NULL != pRosterMgr)
	{
		//Ϊ��֧��android��������Ϣ����Ҫ���Լ����ǳƴ��ϡ�
		tstring strMyAccount = CController::Instance()->GetXmppClient()->jid().bare();
		tstring strMyNickName;
		sUserVCard myCard;
		if(GetUserVCard(strMyAccount, myCard))
		{
			strMyNickName = myCard.strUserNickname;
		}
		else
		{
			strMyNickName = CFileHelper::GetUserName(strMyAccount);
		}
		tstring strNickUtf8 = CChineseCode::EncodeUTF8(strMyNickName);
		
		JID jid(strUserAccount);
		pRosterMgr->SendAddRosterRequest/*subscribe*/(jid, strNickUtf8/*strNick*/, strMsg);
	}
}

void CUserInfoManager::RemoveUser(tstring& strUsrAccount)
{
	RosterManager* pRosterMgr = CController::Instance()->GetXmppClient()->rosterManager();
	if (NULL != pRosterMgr)
	{
		JID jid(strUsrAccount);
		pRosterMgr->remove(jid);
	}

	UpdateCacheAfterFriendRemoved(strUsrAccount);
}

EUsrState CUserInfoManager::GetState(const tstring& strAccount)
{
	EUsrState eState = enmState_Offline;
	::EnterCriticalSection(&m_csRosters);
	MapAllRosters::iterator itFind = m_mapAccount2Rosters.find(strAccount);
	if (itFind != m_mapAccount2Rosters.end())
	{
		eState = itFind->second.eStatus;
	}
	::LeaveCriticalSection(&m_csRosters);

	return eState;
}

bool CUserInfoManager::IsMySelf(const tstring& strAccount)
{
	tstring strTmp = strAccount;
	if (strTmp.empty())
	{
		return false;
	}

	int nIdex = strTmp.find('@', 0);
	if (nIdex == -1)
	{
		strTmp += MMPC_ACCOUNT;//tstring("@test-d/mmpc");
	}

	return (strTmp == CController::Instance()->GetXmppClient()->jid().bare());
}

bool CUserInfoManager::IsMyFriend(const tstring& strAccount)
{
	tstring strTmp = strAccount;
	if (strTmp.empty())
	{
		return false;
	}

	int nIdex = strTmp.find('@', 0);
	if (nIdex == -1)
	{
		strTmp += MMPC_ACCOUNT;//tstring("@test-d/mmpc");
	}

	bool bFriend = false;
	::EnterCriticalSection(&m_csRosters);
	MapAllRosters::iterator itFind = m_mapAccount2Rosters.find(strTmp);
	bFriend = itFind != m_mapAccount2Rosters.end();
	::LeaveCriticalSection(&m_csRosters);

	return bFriend;
}


void CUserInfoManager::RemoveEventHandler(IUserManagerEvent* pHandler)
{
	if (NULL != pHandler)
	{
		::EnterCriticalSection(&m_csListener);
		for (list<IUserManagerEvent*>::iterator it = m_listHandlers.begin();
			it != m_listHandlers.end(); ++it)
		{
			IUserManagerEvent* pLocal = *it;
			if (pLocal == pHandler)
			{
				m_listHandlers.erase(it);
				break;
			}
		}
		::LeaveCriticalSection(&m_csListener);
	}
}

// void CUserInfoManager::AddFriend(tstring& strUsrPhone)
// {
// 	tstring strUserAccount = strUsrPhone + MMPC_DOMAIN;
// 	JID jid(strUserAccount);
// 	Subscription subs(Subscription::S10nType::Subscribe, jid, MMPC_EMPTYSTR, MMPC_EMPTYSTR);
// 	Client* pClient = CController::Instance()->GetXmppClient();
// 	if (NULL != pClient)
// 	{
// 		pClient->send(subs);
// 	}
// }

////server�Ỻ����Щ�ص��¼�����������߿��ܻ��ܵ����ߣ����߼Ӻ��ѳɹ�������֪ͨ��
//void CUserInfoManager::handleSubscription( const Subscription& subscription )
//{
//	//�Է�����Ϊ����
//	tstring strFromAccount = subscription.from().bare();
//	if (Subscription::S10nType::Subscribed == subscription.subtype())
//	{
//		for (list<IUserManagerEvent*>::iterator it = m_listHandlers.begin(); 
//			it != m_listHandlers.end(); ++it)
//		{
//			IUserManagerEvent* pHandler = (IUserManagerEvent*)(*it);
//			if (NULL != pHandler)
//			{
//				pHandler->OnAddFriendSucc(strFromAccount);
//			}
//		}
//	}
//	//�Է�ɾ������
//	else if (Subscription::S10nType::Unsubscribed == subscription.subtype())
//	{
//		for (list<IUserManagerEvent*>::iterator it = m_listHandlers.begin(); 
//			it != m_listHandlers.end(); ++it)
//		{
//			IUserManagerEvent* pHandler = (IUserManagerEvent*)(*it);
//			if (NULL != pHandler)
//			{
//				pHandler->OnFriendRemoved(strFromAccount,false);
//			}
//		}
//	}
//}

void CUserInfoManager::UpdateCacheAfterFriendRemoved(tstring& strAccount)
{
	//����Roster cache
	::EnterCriticalSection(&m_csRosters);
	MapAllRosters::iterator itFind = m_mapAccount2Rosters.find(strAccount);
	if (itFind != m_mapAccount2Rosters.end())
	{
		m_mapAccount2Rosters.erase(itFind);
	}
	::LeaveCriticalSection(&m_csRosters);

	::EnterCriticalSection(&m_csVCards);
	MapAccount2VCards::iterator itVcard = m_mapUser2VCard.find(strAccount);
	if (itVcard != m_mapUser2VCard.end())
	{
		m_mapUser2VCard.erase(itVcard);
	}
	::LeaveCriticalSection(&m_csVCards);
}

void CUserInfoManager::UpdateMemo(const tstring& strAccount, tstring& strNewMemo)
{
	bool bSucc = false;
	//����roter item cache
	::EnterCriticalSection(&m_csRosters);
	MapAllRosters::iterator itFind = m_mapAccount2Rosters.find(strAccount);
	if (itFind != m_mapAccount2Rosters.end())
	{
		bSucc = true;
		itFind->second.strMemo = strNewMemo;
	
		RosterManager* pRosterMgr = CController::Instance()->GetXmppClient()->rosterManager();
		if (NULL != pRosterMgr)
		{
			StringList listGroup;
			//if (!itFind->second.strOrgnization.empty())
			//{
			//	tstring strNewGroup = itFind->second.strOrgID;
			//	strNewGroup += "|";
			//	strNewGroup += itFind->second.strOrgnization;
			//	//listGroup.push_back(CChineseCode::EncodeUTF8(strNewGroup));
			//}

			JID jid(strAccount);
			pRosterMgr->add(jid, CChineseCode::EncodeUTF8(strNewMemo), listGroup);
		}
	}
	::LeaveCriticalSection(&m_csRosters);

	if (!bSucc)
	{
		return;
	}

	//����VCard
	::EnterCriticalSection(&m_csVCards);
	MapAccount2VCards::iterator itFind2 = m_mapUser2VCard.find(strAccount);
	if (itFind2 != m_mapUser2VCard.end())
	{
		itFind2->second.strRemark = strNewMemo;
	}
	::LeaveCriticalSection(&m_csVCards);
}

void CUserInfoManager::FetchGroupMemVCards(vector<tstring>& vecGroupMembers)
{
	//������ڣ��Ͳ��棬��������ڣ���
	int nSize = vecGroupMembers.size();
	if (nSize>0)
	{
		for (int i=0; i<nSize; i++)
		{
			::EnterCriticalSection(&m_csVCards);
			MapAccount2VCards::iterator itFind = m_mapUser2VCard.find(vecGroupMembers[i]);
			//û�о�ȥ��ȥ����
			if (itFind == m_mapUser2VCard.end())
			{
				FetchVCardByAccount(vecGroupMembers[i]);
			}
			::LeaveCriticalSection(&m_csVCards);
		}
		
	}
	
}