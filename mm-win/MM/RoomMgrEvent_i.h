#pragma once

#include <string>
#include <vector>
namespace std{
#ifdef _UNICODE
	typedef wstring tstring;
#else
	typedef string tstring;
#endif
}
class IRoomManagerEvent
{
public:
	virtual void OnGetRoomNameSuccess(const std::tstring& strUniqueRoom) = 0;
	virtual void OnCreateRoomResult(bool bSucc) = 0;
	virtual void OnReceveGroupMessage(const std::tstring& strRoomID, 
									const std::tstring& strSenderAccount, 
									const std::tstring& strSenderNick, 
									const std::tstring& strMsg, bool bPrivate) = 0;
	virtual void OnNewGroupMemberList(const std::tstring& strRoomID, std::vector<std::tstring>& vecMemList) = 0;
	virtual void OnRoomNameArrived(const std::tstring& strRoomID, std::tstring& strRoomNickName) = 0;
	virtual void OnEnterRoomSucess() = 0;
	virtual void OnEnterRoomFailed(std::tstring& strErrorDes) = 0;
};

class CRoomManagerEventBase : public IRoomManagerEvent
{
public:
	virtual void OnGetRoomNameSuccess(const std::tstring& strUniqueRoom) {}
	virtual void OnCreateRoomResult(bool bSucc) {}
	virtual void OnReceveGroupMessage(const std::tstring& strRoomID, const std::tstring& strSenderAccount, 
		const std::tstring& strSenderNick, const std::tstring& strMsg, bool bPrivate){}
	virtual void OnNewGroupMemberList(const std::tstring& strRoomID, std::vector<std::tstring>& vecMemList){}
	virtual void OnRoomNameArrived(const std::tstring& strRoomID, std::tstring& strRoomNickName){}
	virtual void OnEnterRoomSucess() {}
	virtual void OnEnterRoomFailed(std::tstring& strErrorDes) {}
};