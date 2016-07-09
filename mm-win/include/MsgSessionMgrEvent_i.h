#pragma once

#include <string>
#include "..\Common\MMDefine.h"

using namespace std;


class IMsgSessionMgrEvent
{
public:
	virtual void OnChatMsgReceived(tstring& strSenderAccount, tstring& strChatMsg) = 0;
	virtual void OnCmdMsgReceived(tstring& strSender, tstring& strCmd, ECmdType eType) = 0;
	virtual void OnGroupChatMsgReceived(tstring& strRoomID, tstring& strSenderPhone, tstring& strText) = 0;
	virtual void OnRecvOrgReq(tstring strFromJid, tstring strFromNick, tstring strOrgId, 
		tstring strOrgName, tstring strMsgType, tstring strReqId, tstring strDestJid, tstring strDestNick) = 0;
};
