
#pragma once
#include "..\Common\MMDefine.h"

class IMsgCenterModuleEvent;

class IMsgCenterModule
{
public:
	virtual void AddEventHandler(IMsgCenterModuleEvent* pEventHandler) = 0;	
	virtual void RemoveEventHandler(IMsgCenterModuleEvent* pEventHandler) = 0;
	virtual void Release() = 0;

	//∂¡»°Œ¥∂¡œ˚œ¢
	virtual int  GetUnreadMsgCount(tstring strFrombare) = 0;
	virtual bool doHaveUnReadMsg() = 0;
	virtual void GetUnReadTextMsgs(tstring strFrombare, ListMsgs& listAllMsg) = 0;
	virtual void ClearUnReadTextMsgs(tstring strFrombare) = 0;
	virtual void GetUnReadSysMsgs(ListMsgs& listAllMsg) = 0;
	virtual void ClearUnReadSysMsgs()=0;
	virtual bool GetLatestUnReadMsg(tstring& strFrombare, CMsgBase*& pOneMsg) = 0;
	virtual void RemoveLatestUnReadMsg(tstring& strFrombare, __int64 uTime) = 0;
};

//extern "C"
//{
	/*AFX_EXT_API*/ IMsgCenterModule* GetMsgCenterModule();
//}