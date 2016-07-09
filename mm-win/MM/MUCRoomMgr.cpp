#include "MUCRoomMgr.h"
#include "MUCRoomProcessor.h"
#include "Controller.h"
#include "StringHelper.h"
#include "MsgBox.h"
#include "..\include\Global_Def.h"
#include "..\gloox-1.0.14\src\message.h"
#include "..\gloox-1.0.14\src\mucroom.h"
#include "Controller.h"
#include "UserManager.h"

CMUCRoomManager::CMUCRoomManager()
{
	m_pRoomMock = NULL;
	room_list_updated_ = false;
	room_list_deleted_ = false;

	::InitializeCriticalSection(&m_csRooms);
	::InitializeCriticalSection(&m_csRoomItems);
}

CMUCRoomManager::~CMUCRoomManager()
{
	::DeleteCriticalSection(&m_csRooms);
	::DeleteCriticalSection(&m_csRoomItems);
// 	if (NULL != m_pRoom)
// 	{
// 		m_pRoom->removeMUCRoomHandler();
// 		m_pRoom->removeMUCRoomConfigHandler();
// 	}
// 	if (NULL != m_pRoom)
// 	{
// 		delete m_pRoom;
// 		m_pRoom = NULL;
// 	}
}

void CMUCRoomManager::AddHandler(IRoomManagerEvent* pHandler)
{
	if (m_setHandlers.insert(pHandler).second == false)
	{
		//ASSERT(!"�Ѿ���ӹ����¼�������");
	}
}
void CMUCRoomManager::RemoveHandler(IRoomManagerEvent* pHandler)
{
	for (set<IRoomManagerEvent*>::iterator it = m_setHandlers.begin(); 
		it != m_setHandlers.end(); ++it)
	{
		if (*it == pHandler)
		{
			m_setHandlers.erase(it);
			break;
		}
	}
}

void CMUCRoomManager::PreCreateRoom(vector<tstring>& vecParticipants, tstring& strRoomName, const std::tstring nick/*=(_T("")) */)
{
	tstring strTmpname("mmgroupname");
	strTmpname += tstring(MMPC_MUC_DOMAIN);
	strTmpname += "\/";
	strTmpname += nick;

 	JID roomJID( strTmpname );
 	UniqueMUCRoom* pNewRoom = new UniqueMUCRoom( (ClientBase*)CController::Instance()->GetXmppClient(), roomJID, this);
	
	pNewRoom->registerMUCRoomConfigHandler(this);

	RoomItem oneNewRoom;
	oneNewRoom.strFullJID = roomJID.full();
	oneNewRoom.vecMembers = vecParticipants;
	oneNewRoom.strRoomNickName = strRoomName;
	::EnterCriticalSection(&m_csRooms);
	m_mapRooms.insert(make_pair(pNewRoom, oneNewRoom));
	::LeaveCriticalSection(&m_csRooms);

	pNewRoom->RequestUniqueRoomName();

}

void CMUCRoomManager::EnterRoom(const tstring& strRoomID, const tstring& strMyNickname)
{
	UniqueMUCRoom* pNewRoom = NULL;

	if (CheckIfAlreadyExist(strRoomID, pNewRoom))
	{
		//pNewRoom->Join(Presence::Available);
	}
	else
	{
		tstring strFullRoomName = strRoomID;
		strFullRoomName += tstring(MMPC_MUC_DOMAIN);
		strFullRoomName += "\/";
		strFullRoomName += strMyNickname;

		JID roomJID(strFullRoomName);

		/*MUCRoom*/ pNewRoom = new UniqueMUCRoom((ClientBase*)CController::Instance()->GetXmppClient(),
			roomJID, this);

		if (NULL != pNewRoom)
		{
			pNewRoom->registerMUCRoomConfigHandler(this);

			RoomItem oneNewRoom;
			oneNewRoom.strFullJID = roomJID.full();
			oneNewRoom.strRoomID = strRoomID;
			//oneNewRoom.vecMembers = vecParticipants;
			::EnterCriticalSection(&m_csRooms);
			m_mapRooms.insert(make_pair(pNewRoom, oneNewRoom));
			::LeaveCriticalSection(&m_csRooms);
		}
	}

	if (NULL != pNewRoom)
	{
		pNewRoom->getRoomInfo();
		pNewRoom->getRoomItems();

		pNewRoom->setRequestHistory(0, MUCRoom::HistoryRequestType::HistoryMaxStanzas);
		pNewRoom->Join(Presence::Available);
	}
}

void CMUCRoomManager::handleMUCConfigList( MUCRoom* room, const MUCListItemList& items,MUCOperation operation )
{
}

void CMUCRoomManager::handleMUCConfigForm( MUCRoom* room, const DataForm& form )
{
	tstring strRoomName;
	::EnterCriticalSection(&m_csRooms);
	for(map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin();
		it != m_mapRooms.end(); ++it)
	{
		if (room == it->first)
		{
			strRoomName = it->second.strRoomNickName;
		}
	}

	::LeaveCriticalSection(&m_csRooms);

	gloox::DataForm* pNewDataForm = new gloox::DataForm(form);
	pNewDataForm->setType(gloox::FormType::TypeSubmit);
	tstring strUTF8;
	CChineseCode::GB2312ToUTF_8(strUTF8, strRoomName.c_str(), strRoomName.length());
	((gloox::DataFormField*)pNewDataForm->field("muc#roomconfig_persistentroom"))->setValue("1");
	((gloox::DataFormField*)pNewDataForm->field("muc#roomconfig_roomname"))->setValue(strUTF8.c_str());
	((gloox::DataFormField*)pNewDataForm->field("muc#roomconfig_publicroom"))->setValue("1");
	((gloox::DataFormField*)pNewDataForm->field("muc#roomconfig_changesubject"))->setValue("1");

	//����room�µ�����
	room->SetRoomConfigEx( pNewDataForm);
}
void CMUCRoomManager::handleMUCConfigResult( MUCRoom* room, bool success, MUCOperation operation )
{
	//��ʼ��Ⱥ��Ա�ɹ��ˡ�
	if (operation == InitGroupMembers)
	{
		//��ȡ�³�Ա
		room->getRoomItems();

		//
		for (set<IRoomManagerEvent*>::iterator it = m_setHandlers.begin(); 
			it != m_setHandlers.end(); ++it)
		{
			IRoomManagerEvent* pHandler = *it;
			if (NULL != pHandler)
			{
				pHandler->OnCreateRoomResult(success);
			}
		}

		//CMsgBox* pMsgBox = new CMsgBox("Ⱥ������",_T("Ⱥ�����ɹ���"),MSGBOX_ICON_NORMAL, MSGBOX_IDOK);
		//pMsgBox->ShowModal(NULL);
	}
	if (operation == ExitRoom)
	{
		//��ȡ�³�Ա
 		room->getRoomItems();

// 		for (map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin();
// 			it != m_mapRooms.end(); ++it)
// 		{
// 			if (it->first == room)
// 			{
// 				m_mapRooms.erase(it);
// 				break;
// 			}
// 		}
		//�ӱ���map��ɾ��������
 		
// 		for (set<IRoomManagerEvent*>::iterator it = m_setHandlers.begin(); 
// 			it != m_setHandlers.end(); ++it)
// 		{
// 			IRoomManagerEvent* pHandler = *it;
// 			if (NULL != pHandler)
// 			{
// 				pHandler->OnCreateRoomResult(success);
// 			}
// 		}

		room_list_deleted_ = true;
	}
	
}
void CMUCRoomManager::handleMUCRequest( MUCRoom* room, const DataForm& form )
{

}

void CMUCRoomManager::OnRequestRoomResult(MUCRoom* room, bool bSuccess, const std::tstring& strRoomID)
{
	::EnterCriticalSection(&m_csRooms);
	if (bSuccess)
	{
		//lee added.
		for (map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin();
			it != m_mapRooms.end(); ++it)
		{
			if (it->first == room)
			{
				it->second.strRoomID = strRoomID;
				for (set<IRoomManagerEvent*>::iterator it = m_setHandlers.begin(); 
					it != m_setHandlers.end(); ++it)
				{
					IRoomManagerEvent* pHandler = *it;
					if (NULL != pHandler)
					{
						pHandler->OnGetRoomNameSuccess(strRoomID);
					}
				}
				room->RequestRoomConfigEx(strRoomID);
				break;
			}
		}
	}
	else
	{
		//get room name failed.
	}
	::LeaveCriticalSection(&m_csRooms);

}

void CMUCRoomManager::OnCreateRoomResult(MUCRoom* room, bool bSuccess)
{
	::EnterCriticalSection(&m_csRooms);
	if (bSuccess)
	{
		for (map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin();
			it != m_mapRooms.end(); ++it)
		{
			if (it->first == room)
			{
				it->second.bCreated = true;
				room->AddGroupMember(it->second.vecMembers);
				break;
			}
		}
	}
	else
	{

	}
	::LeaveCriticalSection(&m_csRooms);
}

//����������󣬳ɹ��󣬸��±��ص�roomId�µ�Ⱥ��Ա��Ϣ
void CMUCRoomManager::AddRoomNewMember(const tstring& strRoomID, const vector<tstring>& vecNewMembers )
{
	::EnterCriticalSection(&m_csRooms);
	for (map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin(); 
		it != m_mapRooms.end(); ++it)
	{
		//�ҵ�������䣬��ӳ�Ա
		if (_tcsicmp(it->second.strRoomID.c_str(), strRoomID.c_str()) == 0)
		{
			MUCRoom* pRoom = it->first;
			if (NULL != pRoom)
			{
				pRoom->AddGroupMember(vecNewMembers);
			}
		}
	}
	
	::LeaveCriticalSection(&m_csRooms);
}

//������ӳ�Ա����д�˳�Ⱥ��Ϣ
void CMUCRoomManager::ExitFromRoom(const tstring& strRoomID, const vector<tstring>& vecNewMembers )
{
	room_list_deleted_ = false;
	::EnterCriticalSection(&m_csRooms);
 	for (map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin(); 
 		it != m_mapRooms.end(); ++it)
 	{
 		//�ҵ�������䣬�˳���ע��ɾ����ԱҪ�ȷ�������������ɾ
 		if (_tcsicmp(it->second.strRoomID.c_str(), strRoomID.c_str()) == 0)
 		{
 			MUCRoom* pRoom = it->first;
 			if (NULL != pRoom)
 			{
				pRoom->ExitFromGroup(vecNewMembers);
 			}
 		}
 	}
// 	::EnterCriticalSection(&m_csRooms);
// 	for (map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin(); 
// 		it != m_mapRooms.end(); ++it)
// 	{
// 		//�ҵ�������䣬��ӳ�Ա
// 		if (_tcsicmp(it->second.strRoomID.c_str(), strRoomID.c_str()) == 0)
// 		{
// 			MUCRoom* pRoom = it->first;
// 			if (NULL != pRoom)
// 			{
// 				pRoom->leave();
// 			}
// 		}
// 	}
// 
 	::LeaveCriticalSection(&m_csRooms);
}

bool CMUCRoomManager::CheckIfAlreadyExist(const tstring& strRoomID, UniqueMUCRoom*& pRoom)
{
	bool bExist = false;
	::EnterCriticalSection(&m_csRooms);
	for(map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin();
		it != m_mapRooms.end(); ++it)
	{
		RoomItem& oneRoom = it->second;
		if (_tcsicmp(oneRoom.strRoomID.c_str(), strRoomID.c_str()) == 0)
		{
			bExist = true;	
			pRoom = it->first;
		}
	}
	::LeaveCriticalSection(&m_csRooms);

	return bExist;
}

//1.Ⱥ��������ʱ��ᰤ���ϱ�Ⱥ��Ա������״̬��Ϣ��
//2.����Ⱥ��Ա���һ��յ�һ��presence��Ϣ��
// �����޷��ж���1,2�����������������ǵ�2�������mm�ͻ�����Ҫ�ϱ�����OnNewMemberAdded֮��Ļص���֪UI��ȥ���¡�
void CMUCRoomManager::handleMUCParticipantPresence( MUCRoom* room, const MUCRoomParticipant participant,
	const Presence& presence )
{
	::EnterCriticalSection(&m_csRooms);
	for(map<UniqueMUCRoom*, RoomItem>::iterator it =  m_mapRooms.begin();it != m_mapRooms.end(); ++it)
	{
		//�ҵ��������
		if (room == it->first)
		{
			//1.�ȸ���mapAccount2Partpant������Ϣ
			RoomItem& oneRoom = it->second;
			tstring strBare = participant.nick->resource();
			strBare += MMPC_ACCOUNT;//participant.jid != NULL ? participant.jid->bare() : "";
			if (strBare.empty())
			{
				continue;
			}

			map<tstring, MUCRoomParticipant>::iterator itFindpartici = oneRoom.mapAccount2Partpant.find(strBare);
			if (itFindpartici != oneRoom.mapAccount2Partpant.end())
			{
				MUCRoomParticipant& oneParticipant = itFindpartici->second;
				//delete oneParticipant.nick;
				//delete oneParticipant.jid;
				//delete oneParticipant.actor;
				//delete oneParticipant.alternate;

				//oneParticipant.nick = new JID( presence.from());
				oneParticipant.status = presence.status();
				oneParticipant.affiliation = participant.affiliation;
				//oneParticipant.jid = participant.jid != NULL ? new JID(*(participant.jid)) : 0;
				//oneParticipant.actor = participant.actor ? new JID(*participant.actor): 0;
				oneParticipant.reason = participant.reason;
				oneParticipant.newNick = participant.newNick;
				//oneParticipant.alternate = participant.alternate != NULL ? new JID(*participant.alternate) : 0;
				oneParticipant.flags = participant.flags;

				//������Ⱥ���ж�,����������Ⱥ����
				if (participant.affiliation == MUCRoomAffiliation::AffiliationNone && participant.role == RoleNone)
				{
					DeleteMemberFromRoom(room, participant);
//					it->second.vecMembers.erase()
				}
			}
			else
			{
				MUCRoomParticipant party;
				party.nick = new JID( presence.from() );
				party.status = presence.status();
				party.affiliation = participant.affiliation;
				party.role = participant.role;
				party.jid = participant.jid != NULL ? new JID(*(participant.jid)) : 0;
				party.actor = participant.actor ? new JID(*participant.actor): 0;
				party.reason = participant.reason;//mu->reason() ? *(mu->reason()) : EmptyString;
				party.newNick = participant.newNick;//->newNick() ? *(mu->newNick()) : EmptyString;
				party.alternate = participant.alternate != NULL ? new JID(*participant.alternate) : 0;
				party.flags = participant.flags;//mu->flags();

				oneRoom.mapAccount2Partpant.insert(make_pair(strBare, participant));
			}

			//2.�ٸ���mapAccount2State����
			map<tstring, Presence::PresenceType>::iterator itState = oneRoom.mapAccount2State.find(strBare);
			if (itState != oneRoom.mapAccount2State.end())
			{
				itState->second = presence.presence();
			}
			else
			{
				oneRoom.mapAccount2State.insert(make_pair(strBare, presence.presence()));
			}

			//3.�������			
			break;
		}		
	}
	::LeaveCriticalSection(&m_csRooms);
}

void CMUCRoomManager::handleMUCMessage( MUCRoom* room, const Message& msg, bool priv )
{
	for (set<IRoomManagerEvent*>::iterator it = m_setHandlers.begin();
		it != m_setHandlers.end(); ++it)
	{
		IRoomManagerEvent* pHandler = *it;
		if (NULL != pHandler)
		{
			tstring strOut;
			CChineseCode::UTF_8ToGB2312(strOut, (char*)msg.body().c_str(), msg.body().size());

			pHandler->OnReceveGroupMessage(room->name(), msg.from().bare(), msg.from().resource(), strOut, priv);
		}
	}
	
}
bool CMUCRoomManager::handleMUCRoomCreation( MUCRoom* room )
{
	return true;
}
void CMUCRoomManager::handleMUCSubject( MUCRoom* room, const std::tstring& nick,
	const std::tstring& subject )
{

}

void CMUCRoomManager::handleMUCInviteDecline( MUCRoom* room, const JID& invitee,
	const std::tstring& reason )
{

}

void CMUCRoomManager::handleMUCError( MUCRoom* room, StanzaError error )
{
	for (set<IRoomManagerEvent*>::iterator it = m_setHandlers.begin();
		it != m_setHandlers.end(); ++it)
	{
		IRoomManagerEvent* pHandler = *it;
		if (NULL != pHandler)
		{
			tstring strError;
			pHandler->OnEnterRoomFailed(strError);
		}
	}
}

void CMUCRoomManager::HandleRoomNameReturn( MUCRoom* room, int features, const std::tstring& name,
	const DataForm* infoForm) 
{
	tstring strOut;
	CChineseCode::UTF_8ToGB2312(strOut, name.c_str(), name.size());
	
	::EnterCriticalSection(&m_csRoomItems);
	tstring strRoomID;
	map<tstring, sOneRoomItem>::iterator itFind = m_mapRoom2Members.find(strRoomID);
	if (itFind != m_mapRoom2Members.end())
	{
		sOneRoomItem& oneItem = itFind->second;
		oneItem.strRoomNickName = name;
	}
	::LeaveCriticalSection(&m_csRoomItems);
	
}
void CMUCRoomManager::handleMUCInfo( MUCRoom* room, int features, const std::tstring& name,
	const DataForm* infoForm )
{
	tstring strOut;
	CChineseCode::UTF_8ToGB2312(strOut, name.c_str(), name.size());

	::EnterCriticalSection(&m_csRooms);

	for(map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin();
		it != m_mapRooms.end(); ++it)
	{
		if (it->first == room)
		{
			it->second.strRoomNickName = strOut.c_str();
			break;
		}
	}
	::LeaveCriticalSection(&m_csRooms);

	//�½���Ⱥ�������ҵ�Ⱥ�б��������Ӹ�room
	::EnterCriticalSection(&m_csRoomItems);
	map<tstring, sOneRoomItem>::iterator itFindRoom = m_mapRoom2Members.find(room->name());
	if (itFindRoom == m_mapRoom2Members.end())
	{
		sOneRoomItem oneNewRoom;
		oneNewRoom.strRoomID = room->name();
		oneNewRoom.strRoomNickName = strOut;
		//oneNewRoom.vecMember = ���Ժ��Ⱥ��Ա�ص���������vector
		m_mapRoom2Members.insert(make_pair(room->name(), oneNewRoom));
	}
	::LeaveCriticalSection(&m_csRoomItems);

	for (set<IRoomManagerEvent*>::iterator it = m_setHandlers.begin();
		it != m_setHandlers.end(); ++it)
	{
		IRoomManagerEvent* pHandler = *it;
		if (NULL != pHandler)
		{
			pHandler->OnRoomNameArrived(room->name(), strOut);
		}
	}
}

//Ⱥ�б��·��ˡ���
void CMUCRoomManager::HandleRoomList(MUCRoom* room, const Disco::ItemList& items)
{
	::EnterCriticalSection(&m_csRoomItems);
	
	//Ⱥ�б��·������
	if (items.size()>0)
	{
		for (Disco::ItemList::const_iterator it = items.begin(); it != items.end(); ++it)
		{	
			Disco::Item* pOneItem = *it;

			tstring strRoomID   = pOneItem->jid().username();
			tstring strRoomName = pOneItem->name();
			
			if (!strRoomName.empty())
			{
				map<tstring, sOneRoomItem>::iterator itFind = m_mapRoom2Members.find(strRoomID);
				if (itFind == m_mapRoom2Members.end())
				{
					sOneRoomItem oneRoomItem;
					oneRoomItem.strRoomID = strRoomID;
					CChineseCode::UTF_8ToGB2312(oneRoomItem.strRoomNickName, strRoomName.c_str(), strRoomName.length());

					m_mapRoom2Members.insert(make_pair(strRoomID, oneRoomItem));
				}
			}
		}
	}

	//��ȡȺ��Ա
	if (m_mapRoom2Members.size()>0)
	{
		for (map<tstring, sOneRoomItem>::iterator it = m_mapRoom2Members.begin(); 
			it != m_mapRoom2Members.end(); ++it)
		{
			tstring strRoomID = it->first;

			room->GetRoomMembers(strRoomID);
		}
	}
	
	::LeaveCriticalSection(&m_csRoomItems);

	room_list_updated_ = true;//Ⱥ�ı��Ѿ��·�
}

void CMUCRoomManager::HandleRoomMemberList(MUCRoom* room, const Disco::ItemList& items)
{
	::EnterCriticalSection(&m_csRoomItems);

	if (items.size()>0)
	{
		tstring strRoomID;
		tstring strRoomName;
		vector<tstring> vecAllMem;
		for (Disco::ItemList::const_iterator it = items.begin(); it != items.end(); ++it)
		{	
			Disco::Item* pOneItem = *it;
			strRoomID = pOneItem->jid().username();
			vecAllMem.push_back(pOneItem->name());
		}	

		if (vecAllMem.size()>0)
		{
			map<tstring, sOneRoomItem>::iterator itFind = m_mapRoom2Members.find(strRoomID);
			if (itFind != m_mapRoom2Members.end())
			{
				sOneRoomItem& oneItem = itFind->second;
				oneItem.vecMember.clear();

				for (int i=0; i<vecAllMem.size(); i++)
				{
					oneItem.vecMember.push_back(vecAllMem[i]+"@test-d");
				}
			}
			//û�о����
			else
			{
				sOneRoomItem oneRoomItem;
				oneRoomItem.strRoomID = strRoomID;

				for (int i=0; i<vecAllMem.size(); i++)
				{
					oneRoomItem.vecMember.push_back(vecAllMem[i]+"@test-d");
				}

				m_mapRoom2Members.insert(make_pair(strRoomID, oneRoomItem));

				m_pRoomMock->GetRoomInfoEx(strRoomID);
			}
		}
		
	}
	
	::LeaveCriticalSection(&m_csRoomItems);
}

void CMUCRoomManager::handleMUCItems( MUCRoom* room, const Disco::ItemList& items )
{
	::EnterCriticalSection(&m_csRooms);
	for(map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin();
		it != m_mapRooms.end(); ++it)
	{
		if (it->first == room)
		{
			//��վ�����
			it->second.vecMembers.clear();

			//add new data
			for (std::list<gloox::Disco::Item*>::const_iterator itMem = items.begin(); itMem != items.end(); ++itMem)
			{
				gloox::Disco::Item* pItem = *itMem;
				tstring strName = pItem->name().c_str();
				strName += MMPC_ACCOUNT;
				it->second.vecMembers.push_back(strName);
			}

			//Ϊ���ܹ���Ⱥ������show��Ⱥ��Ա��Ϣ�����ܲ����Լ��ĺ��ѣ�����Ҫ����Щİ����Ⱥ��Ա��VCard��ȡ����
			CController::Instance()->GetUserInfoMgr()->FetchGroupMemVCards(it->second.vecMembers);

			tstring strRoomID = room->name();
			UpdateRoomMemberList(strRoomID, it->second.vecMembers);

			for (set<IRoomManagerEvent*>::iterator itHandler = m_setHandlers.begin();
				itHandler != m_setHandlers.end(); ++itHandler)
			{
				IRoomManagerEvent* pHandler = *itHandler;
				if (NULL != pHandler)
				{
					pHandler->OnNewGroupMemberList(it->second.strRoomID, it->second.vecMembers);					
				}
			}

			break;
		}
	}
	::LeaveCriticalSection(&m_csRooms);	
}

void CMUCRoomManager::SendMessage(const tstring& strRoomID, const tstring& strTextMsg)
{
	::EnterCriticalSection(&m_csRooms);
	for(map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin();
		it != m_mapRooms.end(); ++it)
	{
		RoomItem& oneRoom = it->second;
		if (_tcsicmp(strRoomID.c_str(), oneRoom.strRoomID.c_str()) == 0)
		{
			UniqueMUCRoom* pRoom = it->first;
			if (NULL  != pRoom)
			{
				tstring strOut;
				CChineseCode::GB2312ToUTF_8(strOut, (char*)strTextMsg.c_str(), strTextMsg.size());

				pRoom->send(strOut);
				break;
			}
		}
	}
	::LeaveCriticalSection(&m_csRooms);
}

bool CMUCRoomManager::GetRoomMemberList(const tstring& strRoomID, vector<tstring>& vecMemList)
{
	::EnterCriticalSection(&m_csRooms);
	for(map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin();
		it != m_mapRooms.end(); ++it)
	{
		RoomItem& oneRoom = it->second;
		if (_tcsicmp(strRoomID.c_str(), oneRoom.strRoomID.c_str()) == 0)
		{
			vecMemList = oneRoom.vecMembers;
			break;
		}
	}
	::LeaveCriticalSection(&m_csRooms);

	return vecMemList.size()>0;
}

bool CMUCRoomManager::GetRoomMemberListFromRoot(const tstring& strRoomID, vector<tstring>& vecMemList)
{
	::EnterCriticalSection(&m_csRooms);

	for (map<tstring, sOneRoomItem>::iterator it = m_mapRoom2Members.begin(); 
		it != m_mapRoom2Members.end(); ++it)
	{
		if (_tcsicmp(strRoomID.c_str(), it->first.c_str()) == 0)
		{
			vecMemList = it->second.vecMember;
		}
	}
 	
// 	for(map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin();
// 		it != m_mapRooms.end(); ++it)
// 	{
// 		RoomItem& oneRoom = it->second;
// 		if (_tcsicmp(strRoomID.c_str(), oneRoom.strRoomID.c_str()) == 0)
// 		{
// 			vecMemList = oneRoom.vecMembers;
// 			break;
// 		}
// 	}
 	::LeaveCriticalSection(&m_csRooms);
 
 	return vecMemList.size()>0;
}

int CMUCRoomManager::GetRoomMemberCount(const tstring& strRoomId)
{
	int nCount = 0;
	::EnterCriticalSection(&m_csRooms);
	for(map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin();
		it != m_mapRooms.end(); ++it)
	{
		RoomItem& oneRoom = it->second;
		if (_tcsicmp(strRoomId.c_str(), oneRoom.strRoomID.c_str()) == 0)
		{
			nCount = oneRoom.vecMembers.size();
			break;
		}
	}
	::LeaveCriticalSection(&m_csRooms);

	return nCount;
}

// void CMUCRoomManager::GetRoomMember(const tstring& room_id, vector<tstring>& member_id)
// {
// 	int nCount = 0;
// 	::EnterCriticalSection(&m_csRooms);
// 	for(map<UniqueMUCRoom*, RoomItem>::iterator it = m_mapRooms.begin();
// 		it != m_mapRooms.end(); ++it)
// 	{
// 		RoomItem& oneRoom = it->second;
// 		if (_tcsicmp(room_id.c_str(), oneRoom.strRoomID.c_str()) == 0)
// 		{
// 			member_id = oneRoom.vecMembers;
// 			break;
// 		}
// 	}
// 	::LeaveCriticalSection(&m_csRooms);
// }

void CMUCRoomManager::GetMyRoomList()
{
	tstring strMyNick = CController::Instance()->GetXmppClient()->jid().username();

	tstring strTmpname("GetRoomList");
	strTmpname += tstring(MMPC_MUC_DOMAIN);
	strTmpname += "\/";
	strTmpname += strMyNick;

	JID roomJID( strTmpname );
	MUCRoom* pNewRoom = new MUCRoom( (ClientBase*)CController::Instance()->GetXmppClient(), roomJID, this);
	
	m_pRoomMock = pNewRoom;

	pNewRoom->GetRoomList();
}

void CMUCRoomManager::GetMembersByRoomID(tstring& strRoomID)
{
	if (m_pRoomMock != NULL)
	{
		//������Ⱥ���ڣ��ͻ�ȡ�����򣬺���
		if(!IsRoomMembersCached(strRoomID))
		{
			m_pRoomMock->GetRoomMembers(strRoomID);
		}
	}
}
bool CMUCRoomManager::IsRoomMembersCached(tstring& strRoomID)
{
	bool bIsCached = false;

	::EnterCriticalSection(&m_csRoomItems);
	map<tstring, sOneRoomItem>::iterator itFind = m_mapRoom2Members.find(strRoomID);
	if (itFind != m_mapRoom2Members.end())
	{
		bIsCached = true;
		//����е���Ⱥ��Ա����0��ҲҪ����һ��
		if (itFind->second.vecMember.size() == 0)
		{
			bIsCached = false;
		}
	}
	
	::LeaveCriticalSection(&m_csRoomItems);

	return bIsCached;
}
bool CMUCRoomManager::IsInThisRoom(tstring& strRoomID, tstring& strAccount)
{
	bool bInThisRoom = false;

	::EnterCriticalSection(&m_csRoomItems);
	map<tstring, sOneRoomItem>::iterator itFind = m_mapRoom2Members.find(strRoomID);
	if (itFind != m_mapRoom2Members.end())
	{
		sOneRoomItem& oneRoomItem = itFind->second;
		if (oneRoomItem.vecMember.size()>0)
		{
			for (int i=0; i<oneRoomItem.vecMember.size(); i++)
			{
				if (_tcsicmp(strAccount.c_str(), oneRoomItem.vecMember[i].c_str()) == 0)
				{
					bInThisRoom = true;
					break;
				}
			}
		}
	}
	::LeaveCriticalSection(&m_csRoomItems);

	return bInThisRoom;
}

//Ⱥ��Ա���ĵ�ʱ��������Ա���½���Ⱥ��ˢ��Ⱥ��Ա��
void CMUCRoomManager::UpdateRoomMemberList(const tstring& strRoomID, vector<tstring>& vecNewMemList)
{
	::EnterCriticalSection(&m_csRoomItems);
	map<tstring, sOneRoomItem>::iterator itFind = m_mapRoom2Members.find(strRoomID);
	if (itFind != m_mapRoom2Members.end())
	{
		itFind->second.vecMember.clear();
		itFind->second.vecMember = vecNewMemList;
	}

	::LeaveCriticalSection(&m_csRoomItems);
}

void CMUCRoomManager::DeleteMemberFromRoom( MUCRoom* room, const MUCRoomParticipant participant )
{
// 	typedef struct tagRoomItem
// 	{
// 		tstring strFullJID;
// 		tstring strRoomNickName;
// 		tstring strRoomID;
// 		bool   bCreated;
// 		vector<tstring> vecMembers;
// 		map<tstring, MUCRoomParticipant> mapAccount2Partpant;
// 		map<tstring, Presence::PresenceType> mapAccount2State;
// 		tagRoomItem()
// 		{
// 			bCreated = false;
// 		}
// 	}RoomItem;

// 	for(map<UniqueMUCRoom*, RoomItem>::iterator it =  m_mapRooms.begin();it != m_mapRooms.end(); ++it)
// 	{
// 		//�ҵ��������
// 		if (room == it->first)
// 		{
// 			for (vector<tstring>::iterator this_vec = it->second.vecMembers.begin();)
// 			{
// 			}
// 		}
// 	}
}
