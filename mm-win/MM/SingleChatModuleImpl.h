
#pragma once

#include <set>
#include <map>
#include <string>
#include "..\include\SingleChatModule.h"
#include "..\include\MsgCenterEvent_i.h"
#include "chatBoxdlg.h"

using namespace std;

class chatBoxDlg;
class CSingleChatModuleImpl : public ISingleChatModule
							, public IMsgCenterModuleEvent
{
public:
	CSingleChatModuleImpl(void);
	~CSingleChatModuleImpl(void);

	//ISingleChatModule
	virtual void AddEventHandler(ISingleChatModuleEvent* pEventHandler);
	virtual void RemoveEventHandler(ISingleChatModuleEvent* pEventHandler);
	virtual void Release();
	virtual WindowImplBase* ShowSingleChatDlg(string& strUserbare);
	virtual void RemoveSingleChatDlg(string strUserBare);
	virtual BOOL IsDlgCreated(string& strUserbare);

	//IMsgCenterModuleEvent
	virtual void OnReceiveSinglechatTextMsg(string& strFromBare, string& strMsg);

	virtual chatBoxDlg* FindSingleChatDlg(string& strUserBare);
	
	
private:
	typedef std::set<ISingleChatModuleEvent*> CCtrEventHandler;
	CCtrEventHandler m_ctrEventHandler;

	std::map<string,chatBoxDlg*> m_mapSingChatDlg;
	
	void RemoveAllSingleChatDlg();

	void ReadMessage(chatBoxDlg* pChatBoxDlg, string strFrom);
};