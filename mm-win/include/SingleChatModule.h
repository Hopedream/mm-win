#pragma once

class ISingleChatModuleEvent;
class DuiLib::WindowImplBase;

class ISingleChatModule
{
public:
	virtual void AddEventHandler(ISingleChatModuleEvent* pEventHandler) = 0;	// 添加事件处理者
	virtual void RemoveEventHandler(ISingleChatModuleEvent* pEventHandler) = 0;
	virtual void Release() = 0;

	virtual WindowImplBase* ShowSingleChatDlg(string& strUserbase) = 0;
	virtual void RemoveSingleChatDlg(string strUserbase) = 0;
	virtual BOOL IsDlgCreated(string& strUserbase) = 0;
};

ISingleChatModule* GetSingleChatModule();
