#pragma once
#include <windows.h>
#include <set>
#include <vector>
#include <string>
#include "..\include\TrayTaskModule.h"
#include "..\Common\MMDefine.h"
#include "..\include\MsgCenterEvent_i.h"
#include "..\include\UsrManagerEvent_i.h"
#include "..\Common\MMDefine.h"
#include "TrayMgr.h"
#include "MsgWnd.h"

using namespace std;
class CTrayTaskModuleImpl : public ITrayTaskModule
						  ,	public ITrayEvent
						  , public CUserManagerEventBase
						  , public IMsgWndEvent
{
public:
	CTrayTaskModuleImpl(void);
	~CTrayTaskModuleImpl(void);

	virtual void AddEventHandler(ITrayTaskModuleEvent* pEventHandler);
	virtual void Release();
	virtual void Initialize(HINSTANCE hInstance );
	virtual void Uninitialize();
	virtual BOOL IsFlashing();
	virtual void Flash(tstring strAccount, EMsgType eType);
	virtual void FlashGroup(tstring& strRoomId);
	virtual void StopFlash(tstring strAccount, EMsgType eType);
	virtual void StopGroupFlash(tstring& strRoomID);

	//IMsgWnd
	virtual void OnMessage(UINT uMsg, WPARAM w, LPARAM l);


	// ITrayEvent
	virtual void OnTrayEvent(WPARAM w, LPARAM l);

	//IUserManagerEvent
	//virtual void OnRecvAddFriendReq(tstring& strFriend);
	virtual void OnAddFriendSucc(tstring& strFriend);
	virtual void OnFriendRemoved(tstring& strRemovedAccout, bool bActive);

private:

	HICON m_hInstIcon;
	HICON m_hOfflineIcon;
	HICON m_hSysNotifyIcon;
	HICON m_hCallIncommingIcon;
	/**
	* 事件处理者
	*/
	typedef std::set<ITrayTaskModuleEvent*> SetTrayModuleEventHanders;
	SetTrayModuleEventHanders m_setTrayModuleHandlers;

	/**
	* 依赖的服务模块
	*/
	CTrayMgr m_theTrayMgr;

	struct SFlashData
	{
		enum EFlashType
		{
			eFlashType_Null,
			eFlashType_Singlechat,
			eFlashType_SysNotify,
			eFlashType_Group,
			eFlashType_IncomeCall,
		};

		EFlashType eFlashType;
		HICON hicon;

		SFlashData(EFlashType eFlashType, HICON hicon)
		{
			this->eFlashType = eFlashType;
			this->hicon = hicon;
		}

		virtual ~SFlashData()
		{

		}
	};
	//聊天消息提醒。
	struct SSinglechatFlashData : SFlashData
	{
		/*uin*/tstring u;

		SSinglechatFlashData(/*uin*/tstring u, HICON hicon) : SFlashData(eFlashType_Singlechat, hicon)
		{
			this->u = u;
		}
	};
	//系统消息提醒
	struct SSysNotifyFlashData : SFlashData
	{
		tstring strAccount;
		SSysNotifyFlashData(/*uin*/tstring u, HICON hicon) : SFlashData(eFlashType_SysNotify, hicon)
		{
			this->strAccount = u;
		}
	};
	//群消息提醒
	struct SGroupFlashData : SFlashData
	{
		tstring gid;

		SGroupFlashData(tstring gid, HICON hicon) : SFlashData(eFlashType_Group, hicon)
		{
			this->gid = gid;
		}
	};

	struct SCallIncomeFlashData : SFlashData
	{
		tstring strAccount;

		SCallIncomeFlashData(tstring _name, HICON hicon) : SFlashData(eFlashType_IncomeCall, hicon)
		{
			this->strAccount = _name;
		}
	};

	//typedef std::vector<SFlashData*> CCtrFlashData;
	//CCtrFlashData m_flashdata;

protected:
	bool IsGroupMsgFlashing(tstring& strRoomId, int* pidx=NULL);
	bool IsSingleChatFlashing(tstring strUserBare, int* pidx = NULL);
	void StopSinglechatFlash(tstring strUserbare);
	//系统账户可以有多种、？10000,10001？
	bool IsSysMsgFlashing(tstring& strSysAccount,int* pidx = NULL);
	void StopLatestSysMsgFlash(tstring& strSysAccount);
	void StopIncomeCallMsgFlash(tstring& strAccount);
	bool IsCallIncomeMsgFlashing(tstring& strSysAccount, int* pidx /*= NULL*/);
};