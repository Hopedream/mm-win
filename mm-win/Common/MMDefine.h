#pragma once

#include <vector>
#include <list>
#include <string>
#include <time.h>

typedef int uin ;
typedef int GroupId;
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
enum ECmdType
{
	enmCmd_Undefied	= -1,	//undefined
	enmCmd_FileReq	= 0,	//fileRequest
	enmCmd_ImgReq = 3,
	enmCmd_VoiceReq = 4,
	enmCmd_GroupPackFile = 1,				//发送群文件资料包命令：协议：strPackID,strRoomID
	enmCmd_GroupQianyuePack = 2,		//群文件同意签约包状态：协议：strPackID,strRoomID,strAgreeOrDeny
};

enum EMsgType
{
	enmMsgType_Chat			= 0,	//聊天消息。
	//enmMsgType_CMD	= 1,		//命令消息。
	enmMsgType_Sys			= 2,	//系统消息。
	enmMsgType_File         =6,
	enmMsgType_Image = 7,
	enmMsgType_incomeCall	= 3,	
	enmMsgType_Group		= 4,	//群消息
	enmMsgType_Voice        =5,     //语音消息
};

//命令消息的定义
struct sCmdMsg
{
	tstring strFrom;
	ECmdType eType;
	tstring strParas;
	__int64 uTime;
};

//消息基类
class CMsgBase
{
public:
	CMsgBase()
	{
		strFromBare.empty();
		strMsg.empty();
		uTime = 0;
		eType = enmMsgType_Chat;
	}
	virtual ~CMsgBase(){}

public:
	__int64 uTime;
	EMsgType eType;
	tstring strFromBare;
	tstring strMsg;
};

typedef std::list<CMsgBase*> ListMsgs;

//文本消息
class CTextMsg : public CMsgBase
{
public:
	CTextMsg()
	{ 
		eType = enmMsgType_Chat;
	}
	virtual ~CTextMsg(){}
};

//系统消息的子类型
enum ESysMsgSubType
{
	enmSysMsgSubType_UnKnown	 = -1,
	enmSysMsgSubType_AddFriend	 = 0,
	enmSysMsgSubType_RmvFriend	 = 1,
	enmSysMsgSubType_FileRequest = 2,
	enmSysMsgSubType_VoiceRequest = 6,
	enmSysMsgSubType_AcceptAddFriend = 3,
	enmSysMsgSubType_OrgRequest = 4,
	enmSysMsgSubType_AddOrgFriend = 5,
};

typedef struct tagSysMsg
{
	tstring strReqId;
	tstring strSender;
	tstring strMsg;
	tstring strExInfo;
	ESysMsgSubType eSubType;
	int uTime;
	bool bProcessed;//
	
	tagSysMsg()
	{
		uTime = 0;
		strSender.empty();
		strMsg.empty();
		strExInfo.empty();
		strReqId.empty();
		bProcessed = false;
		eSubType = enmSysMsgSubType_UnKnown;
	}
}sSysMsg;
//系统消息基类
class CSysMsg : public CMsgBase
{
public:
	CSysMsg()
	{
		eType = enmMsgType_Sys;
	}
	virtual ~CSysMsg(){}

public:
	ESysMsgSubType eSubType;
};

//加好友请求的系统消息
class CSysMsgAddFriend : public CSysMsg
{
public:
	CSysMsgAddFriend()
	{
		eSubType = enmSysMsgSubType_AddFriend;
	}
	virtual ~CSysMsgAddFriend(){}

public:
	tstring strSender;
};

//同意添加好友的系统消息
class CSysMsgAcceptAddFriend : public CSysMsg
{
public:
	CSysMsgAcceptAddFriend()
	{
		eSubType = enmSysMsgSubType_AcceptAddFriend;
	}
	virtual ~CSysMsgAcceptAddFriend(){}

public:
	tstring strSender;
};

//删好友后的系统提醒消息
class CSysMsgRemvFriend : public CSysMsg
{
public:
	CSysMsgRemvFriend()
	{
		eSubType = enmSysMsgSubType_RmvFriend;
	}
	virtual ~CSysMsgRemvFriend(){}
public:
	tstring strRemoved;
};

// pubsub org相关消息
class CSysMsgOrgRequest : public CSysMsg
{
public:
	CSysMsgOrgRequest()
	{
		eSubType = enmSysMsgSubType_OrgRequest;
	}
	virtual ~CSysMsgOrgRequest(){}
public:
	tstring strOrgId;
};

class CSysMsgFileRequest : public CSysMsg
{
public:
	CSysMsgFileRequest()
	{
		eSubType = enmSysMsgSubType_FileRequest;
	}
	virtual ~CSysMsgFileRequest(){}
public:

};

class CSysMsgVoiceRequest : public CSysMsg
{
public:
	CSysMsgVoiceRequest()
	{
		eSubType = enmSysMsgSubType_VoiceRequest;
	}
	virtual ~CSysMsgVoiceRequest(){}
public:

};

//系统消息基类
class CGroupMsg : public CMsgBase
{
public:
	CGroupMsg()
	{
		eType = enmMsgType_Group;
	}
	virtual ~CGroupMsg(){}
public:
	tstring strRoomID;
};


enum eSendMsgMode{
	eEnterKey,
	eCtrlEnterKey
};