#pragma once

/**
*  该模块的事件接口
*/
class ITrayTaskModuleEvent
{
public:
	enum ETrayEvent
	{
		eTrayEvent_LBUTTONDOWN		= WM_LBUTTONDOWN,
		eTrayEvent_LBUTTONDBLCLK	= WM_LBUTTONDBLCLK,
		eTrayEvent_RBUTTONDOWN		= WM_RBUTTONDOWN,
		eTrayEvent_LBUTTONDOWN_Late = WM_USER + 8765,
		eTrayEvent_TrayFlashStopped,
	};
	//virtual void OnTrayEvent(WPARAM w, LPARAM l) = 0;

	//告知鼠标双击了，observer需要读取未读消息
	virtual void OnDbClickChatMsg(tstring strFromBare) = 0;
	virtual void OnDbClickSysMessage(tstring strNewUser) = 0;
	virtual void OnDbClickCallIncomeMessage(tstring strFrom) = 0;
	virtual void OnDbClickGroupMsg(tstring strRoom) = 0;
};
class CTrayTaskEventBase : public ITrayTaskModuleEvent
{
	virtual void OnDbClickChatMsg(tstring strFromBare) {}
	virtual void OnDbClickSysMessage(tstring strNewUser){}
	virtual void OnDbClickCallIncomeMessage(tstring strFrom) {}
	virtual void OnDbClickGroupMsg(tstring strRoom){}
};