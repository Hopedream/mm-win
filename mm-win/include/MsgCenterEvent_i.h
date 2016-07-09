#pragma once

#include "..\Common\MMDefine.h"
#include <string>

/**
* 该接口接收从session模块接收上来的原始消息,
* 该接口需要支持的信息应该包括：来自谁，发送的内容（文本消息，系统表情消息，自定义表情，Image）
* 目前该接口只支持文本消息！需要扩展。
*/
class IMsgCenterModuleEvent
{
public:
	virtual void OnReceiveMessage(CMsgBase* pRecvMsg, EMsgType eMsgType) = 0;
	//virtual void OnReceiveCmdMsg(tstring& strFrom, ECmdType eType, tstring& strParas) = 0;
};
