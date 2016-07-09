#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <Windows.h>
#include "RoomMgrEvent_i.h"
#include "..\gloox-1.0.14\src\uniquemucroom.h"
#include "..\gloox-1.0.14\src\mucroomconfighandler.h"
#include "..\gloox-1.0.14\src\mucroomhandler.h"

using namespace std;
using namespace gloox;
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
class gloox::MUCRoom;
class gloox::MUCRoomParticipant;
class CMUCRoomProcessor;

typedef struct tagRoomItem
{
	tstring strFullJID;
	tstring strRoomNickName;
	tstring strRoomID;
	bool   bCreated;
	vector<tstring> vecMembers;
	map<tstring, MUCRoomParticipant> mapAccount2Partpant;
	map<tstring, Presence::PresenceType> mapAccount2State;
	tagRoomItem()
	{
		bCreated = false;
	}
}RoomItem;

class CMUCRoomManager : public MUCRoomConfigHandler
					  , public MUCRoomHandler
{
public:
	CMUCRoomManager();
	virtual ~CMUCRoomManager();

	void AddHandler(IRoomManagerEvent* pHandler);
	void RemoveHandler(IRoomManagerEvent* pHandler);

	//MUCRoomConfigHandler
	virtual void handleMUCConfigList( MUCRoom* room, const MUCListItemList& items,MUCOperation operation );
	virtual void handleMUCConfigForm( MUCRoom* room, const DataForm& form );
	virtual void handleMUCConfigResult( MUCRoom* room, bool success, MUCOperation operation );
	virtual void handleMUCRequest( MUCRoom* room, const DataForm& form );
	virtual void OnRequestRoomResult(MUCRoom* room, bool bSuccess, const std::tstring& strRoomName);
	virtual void OnCreateRoomResult(MUCRoom* room, bool bSuccess);
	
	//MUCRoomHandler
	virtual void handleMUCParticipantPresence( MUCRoom* room, const MUCRoomParticipant participant,const Presence& presence );
	virtual void handleMUCMessage( MUCRoom* room, const Message& msg, bool priv );
	virtual bool handleMUCRoomCreation( MUCRoom* room );
	virtual void handleMUCSubject( MUCRoom* room, const std::tstring& nick,const std::tstring& subject );
	virtual void handleMUCInviteDecline( MUCRoom* room, const JID& invitee,const std::tstring& reason );
	virtual void handleMUCError( MUCRoom* room, StanzaError error );
	virtual void handleMUCInfo( MUCRoom* room, int features, const std::tstring& name,const DataForm* infoForm );
	virtual void HandleRoomNameReturn( MUCRoom* room, int features, const std::tstring& name,const DataForm* infoForm);
	virtual void handleMUCItems( MUCRoom* room, const Disco::ItemList& items );
	virtual void HandleRoomList(MUCRoom* room, const Disco::ItemList& items);
	virtual void HandleRoomMemberList(MUCRoom* room, const Disco::ItemList& items);


	//����serverΨһ��RoomID
	void PreCreateRoom(vector<tstring>& vecParticipants, tstring& strRoomName, const std::tstring nick=(_T("")));

	//��Ⱥ
	void EnterRoom(const tstring& strRoomID, const tstring& strMyNickname);	

	//����Ⱥ��Ϣ
	void SendMessage(const tstring& strRoomID, const tstring& strTextMsg);

	//���ε�½����ȡ�ҵ�Ⱥ�б�
	bool GetRoomMemberList(const tstring& strRoomID, vector<tstring>& vecMemList);

	bool GetRoomMemberListFromRoot(const tstring& strRoomID, vector<tstring>& vecMemList);

	//��ȡ��ǰȺ�ĳ�Ա����
	int GetRoomMemberCount(const tstring& strRoomId);

	//��ȡ��ǰȺ�ĳ�Ա�б�
//	void GetRoomMember(const tstring& room_id, vector<tstring>& member_id);
	//
	void AddRoomNewMember(const tstring& strRoomID, const vector<tstring>& vecNewMembers );

	//��Ⱥ
	void ExitFromRoom(const tstring& strRoomID, const vector<tstring>& vecNewMembers);//by zyy

	//
	void DeleteMemberFromRoom(MUCRoom* room, const MUCRoomParticipant participant);

	//��ȡ�ҵ�Ⱥ�б�
	void GetMyRoomList();

	//��ȡָ��Ⱥ�ĳ�Ա�б�
	void GetMembersByRoomID(tstring& strRoomID);

	//�ж����ڲ������Ⱥ����
	bool IsInThisRoom(tstring& strRoomId, tstring& strAccount);

	void ResetListUpdateState(){room_list_updated_ = false;}

	bool GetListUpdateState(){return room_list_updated_;}

	bool GetListDeleteState(){return room_list_deleted_;}
protected:
	bool IsRoomMembersCached(tstring& strRoomID);
	bool CheckIfAlreadyExist(const tstring& strRoomID, UniqueMUCRoom*& pRoom);
	void UpdateRoomMemberList(const tstring& strRoomID, vector<tstring>& vecNewMemList);

protected:
	map<UniqueMUCRoom*, RoomItem> m_mapRooms;
	CRITICAL_SECTION m_csRooms;

	set<IRoomManagerEvent*> m_setHandlers;

	//����ҵ�Ⱥ����Ⱥ����ĳ�Ա�б�������(��Ⱥ)��ʱ����Ҫ�������cache;

	CRITICAL_SECTION m_csRoomItems;
	struct sOneRoomItem
	{
		tstring strRoomID;
		tstring strRoomNickName;
		vector<tstring> vecMember;
	};
	map<tstring, sOneRoomItem> m_mapRoom2Members;


	//���ڻ�ȡȺ�б��Ⱥ��Ա����ʱȺ����
	MUCRoom* m_pRoomMock;

	//Ⱥ��ӳ�Ա����ô���������Ի�����أ����ҵ�����֮����Ҫ�������������ܸĳɺ�Ⱥ��ӳ�Աһ��
	bool room_list_updated_;//Ⱥ�б��Ƿ��Ѿ��·���by zyy �޷��õ�Ⱥ�б��·������ֻ��������Э
	bool room_list_deleted_;//Ⱥ�б�ɾ���ɹ���
};
