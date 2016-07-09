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


	//请求server唯一的RoomID
	void PreCreateRoom(vector<tstring>& vecParticipants, tstring& strRoomName, const std::tstring nick=(_T("")));

	//进群
	void EnterRoom(const tstring& strRoomID, const tstring& strMyNickname);	

	//发送群消息
	void SendMessage(const tstring& strRoomID, const tstring& strTextMsg);

	//初次登陆，获取我的群列表
	bool GetRoomMemberList(const tstring& strRoomID, vector<tstring>& vecMemList);

	bool GetRoomMemberListFromRoot(const tstring& strRoomID, vector<tstring>& vecMemList);

	//获取当前群的成员个数
	int GetRoomMemberCount(const tstring& strRoomId);

	//获取当前群的成员列表
//	void GetRoomMember(const tstring& room_id, vector<tstring>& member_id);
	//
	void AddRoomNewMember(const tstring& strRoomID, const vector<tstring>& vecNewMembers );

	//退群
	void ExitFromRoom(const tstring& strRoomID, const vector<tstring>& vecNewMembers);//by zyy

	//
	void DeleteMemberFromRoom(MUCRoom* room, const MUCRoomParticipant participant);

	//获取我的群列表
	void GetMyRoomList();

	//获取指定群的成员列表
	void GetMembersByRoomID(tstring& strRoomID);

	//判断我在不在这个群里面
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

	//存放我的群，和群下面的成员列表：当增加(退群)的时候，需要更新这个cache;

	CRITICAL_SECTION m_csRoomItems;
	struct sOneRoomItem
	{
		tstring strRoomID;
		tstring strRoomNickName;
		vector<tstring> vecMember;
	};
	map<tstring, sOneRoomItem> m_mapRoom2Members;


	//用于获取群列表和群成员的临时群对象
	MUCRoom* m_pRoomMock;

	//群添加成员是怎么反馈到主对话框的呢？，找到线索之后需要将下面两个功能改成和群添加成员一致
	bool room_list_updated_;//群列表是否已经下发，by zyy 无法得到群列表下发结果，只能做此妥协
	bool room_list_deleted_;//群列表删除成功；
};
