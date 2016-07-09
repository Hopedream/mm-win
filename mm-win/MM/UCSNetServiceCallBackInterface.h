#pragma once
#include "ucpaas/UCSClient.h"
#include "../include/Global_Def.h"
#include <list>
#include <map>
#include <string>
using namespace std;

struct UCS_Client
{
	char strClient[64];
	char strPwd[64];
	char strPhone[64];
};
struct UCS_Sid
{
	char strSid[64];
	char strToken[64];
	UCS_Client client[6];
};


struct stUcsErrorCodeMap
{
	UINT uErrorCode;
	tstring strErrInfo;
};

struct stUcsIncommingCall{
	tstring strCallerNumber;
	tstring strUserData;
	tstring strCallid;
	UCS_DIALTYPE eCallType;
};

#define UCS_ILLEGAL_ACCOUNT 201301
#define UCS_NAME_PASSWORD_ERROR 201302
#define UCS_CLIENT_ILLEGAL 201303
#define UCS_CLIENT_CLOSED 201304
#define UCS_LOGIN_FAILED 201305
#define UCS_LOGIN_TOKEN_ERROR 201306
#define UCS_LOGIN_CONFLICT 201307
#define UCS_LOGIN_EXCCEPTION_FAILED 201308
#define UCS_SERVER_OFFLINE 201309
#define UCS_INSUFFICIENT_FUNDS 202301
#define UCS_CALLED_BUSY 202302
#define UCS_CALLED_OFFLINE 202303
#define UCS_PHONENUM_ERROR 202304
#define UCS_CALLED_REPLY_TIMEOUT 202305
#define UCS_CALLED_ANSWER_TIMEOUT 202306
#define UCS_DISABLE_CALL 202307
#define UCS_ACCOUNT_SUSPENDED 202308
#define UCS_CALLER_HANGUP 202311
#define UCS_CALLED_REFUSED 202312
#define UCS_CALLED_HANGUP 202313
#define UCS_ARREARAGE_HANGUP  202314
#define UCS_UPLOAD_FAILED 203301
#define UCS_DOWNLOAD_FAILED 203302


const stUcsErrorCodeMap g_stUcsErrCodeMap[] = {
	{UCS_ILLEGAL_ACCOUNT, _T("开发者账号或者密码为非法")}, 
	{UCS_NAME_PASSWORD_ERROR, _T("用户名或者密码错误")}, 
	{UCS_CLIENT_ILLEGAL, _T("Client用户不存在")}, 
	{UCS_CLIENT_CLOSED, _T("Client状态非法(Client状态为关闭状态)  ")}, 
	{UCS_LOGIN_FAILED, _T(" 登录被拒绝（开发者账号状态关闭） ")},    
	{UCS_LOGIN_TOKEN_ERROR , _T("密文登录时Token错误")},
	{ UCS_LOGIN_CONFLICT, _T("用户登录冲突")},
	{ UCS_LOGIN_EXCCEPTION_FAILED, _T("登录异常失败，需要重新登录")},
	{UCS_SERVER_OFFLINE , _T("服务器强制下线")},
	{UCS_INSUFFICIENT_FUNDS , _T("余额不足")},
	{ UCS_CALLED_BUSY, _T("被叫忙")},
	{ UCS_CALLED_OFFLINE, _T("被叫号码不存在或不在线")},
	{UCS_PHONENUM_ERROR , _T("号码错误")},
	{UCS_CALLED_REPLY_TIMEOUT , _T("被叫超时不应答")},
	{ UCS_CALLED_ANSWER_TIMEOUT, _T("被叫接听超时")},
	{UCS_DISABLE_CALL , _T("禁止呼叫")},
	{ UCS_ACCOUNT_SUSPENDED, _T("账户冻结")},
	{ UCS_CALLER_HANGUP, _T("主叫挂断")},
	{ UCS_CALLED_REFUSED, _T("被叫拒接")},
	{ UCS_CALLED_HANGUP, _T("被叫挂断")},
	{ UCS_ARREARAGE_HANGUP, _T("余额不足挂断")},
	{UCS_UPLOAD_FAILED , _T("上传失败")},
	{UCS_DOWNLOAD_FAILED , _T("下载失败")},
};



enum eUcpaasNotify{
	eConnectSuccess = 0,
	eConnectFailed,
	eHangup, 
	eAlerting,
	eAnswer,
	eDialFailed,
	eIncomingCall,
	eUserStatus,
	eCallback,
	eExStatus,
};

enum eExCallStatus{
	eLackCamera = 1,
};

void onConnected();
void onConnectError(int reason);
void onIncomingCall(const char *callid, int callType, const char *callerNumber, const char *callerName,const char *userData);
void onAlerting(const char *callid);
void onAnswer(const char *callid);
void onDialFailed(const char *callid, int reason);
void onHangUp(const char *callid, int reason);
void onReceiveIMessage(UCS_INSTANTMESSAGE *data);
void onSendIMessage(int reason, UCS_INSTANTMESSAGE *data);
void onDownloadFile(int reason, UCS_DOWNLOADINFO *data);
void onQueryUserState(int reason, UCS_ONLINESTATUS *data, int size);
void InitUCSNetServiceCallBackInterface();
void FreeUCSNetServiceCallBackInterface();

UCSCLIENT_API int GetClient(const char *user, const char *pwd, UCS_Sid* sidInfo);
typedef map<tstring,tstring> stringMap;
typedef map<tstring,tstring>::iterator stringMapIter;
class CUcPaasClient
{
public:	
	static CUcPaasClient* Instance();
	bool ConnectUcPaas(const tstring strPhone=(_T("")));	
	bool IsUcPaasContact(const tstring strContactPhone);
	tstring GetPhoneFromCallId(const tstring strCallId);
	 static tstring GetUcsErrInfo( int nErrCode );
	 bool QueryUcsClientPhoneNo(const tstring& strClientId, tstring& strPhoneNo);
	 bool IsUcsConnected();
	 bool SetRingTone(const char *filePath);
	 bool SetRingBackTone(const char *filePath);
	 bool SetVolumeValue(unsigned int value);
	 unsigned int GetVolumeValue();
	 void SetMicMuteStatus(bool bMicMute);
	 bool CloseVideo(int mode);
	 bool OpenVideo(int mode);
	 bool AnswerCall(const char *callid);
	 bool RejectCall(const char *callid);
	 bool IsConnecting() {return m_bIsConneting;}
	 void SetCallRingTone();
	 int GetCameraNum();
	 void SendExCallStatus(const tstring strPhoneNo, const eExCallStatus eStatus);
protected:
	bool GetUcpaasClientIdToken(tstring& strClientId, tstring& strToken);
	bool ConnectUcPaasClientDemo(tstring strPhone);
	bool IsUcPaasClient(const tstring strContactPhone);
	CUcPaasClient();
	bool GetRootAccount(tstring& strSid, tstring& strToken);		
private:
	stringMap m_mapCallId;
	bool m_bIsUcPaasDemo;
	static CUcPaasClient* m_lpInstance;
	bool m_bIsConneting;
};