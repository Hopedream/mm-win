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
	{UCS_ILLEGAL_ACCOUNT, "�������˺Ż�������Ϊ�Ƿ�"}, 
	{UCS_NAME_PASSWORD_ERROR, "�û��������������"}, 
	{UCS_CLIENT_ILLEGAL, "Client�û�������"}, 
	{UCS_CLIENT_CLOSED, "Client״̬�Ƿ�(Client״̬Ϊ�ر�״̬)  "}, 
	{UCS_LOGIN_FAILED, " ��¼���ܾ����������˺�״̬�رգ� "},    
	{UCS_LOGIN_TOKEN_ERROR , "���ĵ�¼ʱToken����"},
	{ UCS_LOGIN_CONFLICT, "�û���¼��ͻ"},
	{ UCS_LOGIN_EXCCEPTION_FAILED, "��¼�쳣ʧ�ܣ���Ҫ���µ�¼"},
	{UCS_SERVER_OFFLINE , "������ǿ������"},
	{UCS_INSUFFICIENT_FUNDS , "����"},
	{ UCS_CALLED_BUSY, "����æ"},
	{ UCS_CALLED_OFFLINE, "���к��벻���ڻ�����"},
	{UCS_PHONENUM_ERROR , "�������"},
	{UCS_CALLED_REPLY_TIMEOUT , "���г�ʱ��Ӧ��"},
	{ UCS_CALLED_ANSWER_TIMEOUT, "���н�����ʱ"},
	{UCS_DISABLE_CALL , "��ֹ����"},
	{ UCS_ACCOUNT_SUSPENDED, "�˻�����"},
	{ UCS_CALLER_HANGUP, "���йҶ�"},
	{ UCS_CALLED_REFUSED, "���оܽ�"},
	{ UCS_CALLED_HANGUP, "���йҶ�"},
	{ UCS_ARREARAGE_HANGUP, "����Ҷ�"},
	{UCS_UPLOAD_FAILED , "�ϴ�ʧ��"},
	{UCS_DOWNLOAD_FAILED , "����ʧ��"},
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
	bool ConnectUcPaas(const tstring strPhone = "");	
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