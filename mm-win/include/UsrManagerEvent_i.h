#pragma once


#include <string>
#include "..\Common\UserInfoDef.h"

using namespace std;
class IUserManagerEvent
{
public:
	virtual void OnMySelfStateUpdate(tstring& strAccount, EUsrState eState, 
										tstring strRes, tstring strBusyInfo=(_T(""))) = 0;
	virtual void OnUserStateUpdated(tstring& strAccount, EUsrState eState, 
									tstring strRes, tstring strBusyInfo=(_T(""))) = 0;
	virtual void OnRosterAvalible(MapAllRosters& listUserInfos)	= 0 ;

	virtual void OnMySelfCardInfo(sUserVCard& oneInfo) = 0;
	virtual void OnVCardReturned(tstring& strAccount, sUserVCard& oneInfo) = 0;
	virtual void OnStrangerVCard(tstring& strAccount, sUserVCard& oneCard) = 0;
	virtual void OnStoreVCardResult(tstring& strAccount, bool bSucc) = 0;

	virtual void OnRecvAddFriendReq(tstring& strFriend, const tstring& strMsg=(_T("")), const tstring& strSenderNick=(_T("")))=0;
	virtual void OnRecvAcceptAddFriendReq(tstring& strFriend, const tstring& strSenderNick=(_T("")))=0;
	virtual void OnAddFriendSucc(tstring& strFriend)=0;
	virtual void OnFriendRemoved(tstring& strRemovedAccout, bool bActive)=0;
};


class CUserManagerEventBase : public IUserManagerEvent
{
	virtual void OnMySelfStateUpdate(tstring& strAccount, EUsrState eState, 
		tstring strRes, tstring strBusyInfo=(_T(""))) {};
	virtual void OnUserStateUpdated(tstring& strAccount, EUsrState eState, 
		tstring strRes, tstring strBusyInfo=(_T(""))) {};
	virtual void OnRosterAvalible(MapAllRosters& listUserInfos)	{};

	virtual void OnMySelfCardInfo(sUserVCard& oneInfo) {};
	virtual void OnVCardReturned(tstring& strAccount, sUserVCard& oneInfo) {};
	virtual void OnStrangerVCard(tstring& strAccount, sUserVCard& oneCard) {};
	virtual void OnStoreVCardResult(tstring& strAccount, bool bSucc) {};

	virtual void OnRecvAddFriendReq(tstring& strFriend, const tstring& strMsg=(_T("")), const tstring& strSenderNick=(_T(""))){};
	virtual void OnRecvAcceptAddFriendReq(tstring& strFriend, const tstring& strSenderNick=(_T(""))){};
	virtual void OnAddFriendSucc(tstring& strFriend){};
	virtual void OnFriendRemoved(tstring& strRemovedAccout, bool bActive){};
};
