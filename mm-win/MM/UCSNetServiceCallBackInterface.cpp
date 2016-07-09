#include "StdAfx.h"
#include "UCSNetServiceCallBackInterface.h"
#include "ucpaas/UCSClient.h"
//#include "mainDlg.h"
#include "json/json.h"
#include "Controller.h"
#include "HttpClient.h"
#include "FileHelper.h"
#include "../Common/Logger.h"
#include <fstream>
#include "MsgBox.h"
#include "videoDlg.h"
//#include "AudioCallIncomingDlg.h"
#include "userInfoDlg.h"
#include "DBTaskModuleImpl.h"
#include "StringHelper.h"


static bool g_bInitUCSNet = false;
const TCHAR* const UCPAAS_CFG_FILE = "config\\ucpaas.cfg";
void onConnected()
{
	HWND hWnd = CVideoDlg::Instance()->GetHWND();
	if (::IsWindow(hWnd))
	{
		CVideoDlg::Instance()->PostMessage(WM_UCPAAS_NOTIFY, WPARAM(eConnectSuccess));
	}
}

void onConnectError(int reason)
{
// 	if (CVideoDlg::Instance()->GetHWND() != NULL)
// 	{
// 		CVideoDlg::Instance()->PostMessage(WM_UCPAAS_NOTIFY, WPARAM(eConnectFailed), reason);
// 	}
// 	
}



void onQueryUserState(int reason, UCS_ONLINESTATUS *data, int size)
{
	if (CVideoDlg::Instance()->IsActive())
	{
		CVideoDlg::Instance()->PostMessage(WM_UCPAAS_NOTIFY, WPARAM(eUserStatus), data->state);
	}
}

void onIncomingCall(const char *callid, UCS_DIALTYPE callType, const char *callerNumber, const char *callerName,const char *userData)
{
	stUcsIncommingCall* lpIncommingInfo = new stUcsIncommingCall;
	lpIncommingInfo->eCallType = callType;
	lpIncommingInfo->strCallerNumber = callerNumber;
	lpIncommingInfo->strCallid = callid;
	CVideoDlg::Instance()->PostMessage(WM_UCPAAS_NOTIFY, eIncomingCall, (LPARAM)lpIncommingInfo);	
}

void onAlerting(const char *callid)
{
	if (CVideoDlg::Instance()->IsActive())
	{
		CVideoDlg::Instance()->PostMessage(WM_UCPAAS_NOTIFY, eAlerting, (LPARAM)callid);
	}
}

void onAnswer(const char *callid)
{
	 if (CVideoDlg::Instance()->IsActive())
	{
		CVideoDlg::Instance()->PostMessage(WM_UCPAAS_NOTIFY, eAnswer, (LPARAM)callid);
	}
}

void onDialFailed(const char *callid, int reason)
{
	if (CVideoDlg::Instance()->IsActive())
	{
		CVideoDlg::Instance()->PostMessage(WM_UCPAAS_NOTIFY, eDialFailed, reason);
	}
}


void onHangUp(const char *callid, int reason)
{
	if (CVideoDlg::Instance()->IsActive())
	{
		CVideoDlg::Instance()->PostMessage(WM_UCPAAS_NOTIFY, eHangup, reason);
	}
}

void onSendIMessage(int reason, UCS_INSTANTMESSAGE *data)
{
	if(reason != 0)
	{

	}
}

void onDownloadFile(int reason, UCS_DOWNLOADINFO *data)
{
	if(reason != 0)
	{

	}
}

void onReceiveIMessage(UCS_INSTANTMESSAGE *data)
{
	if (CVideoDlg::Instance()->IsActive())
	{
		CVideoDlg::Instance()->PostMessage(WM_UCPAAS_NOTIFY, eExStatus, (LPARAM)data->type);
	}
}

void onCallBack(int reason)
{
		CVideoDlg::Instance()->PostMessage(WM_UCPAAS_NOTIFY, WPARAM(eCallback), reason);
}


void InitUCSNetServiceCallBackInterface()
{
	if (g_bInitUCSNet)
	{
		return;
	}
	UCS_CALLBACKINTERFACE callback;
	memset(&callback, 0, sizeof(UCS_CALLBACKINTERFACE));
	callback.onAlerting = &onAlerting;
	callback.onAnswer = &onAnswer;
	callback.onDialFailed = &onDialFailed;
	callback.onHangUp = &onHangUp;
	callback.onIncomingCall = &onIncomingCall;

	callback.onCallBack = &onCallBack;

	callback.onConnectionSuccessful = &onConnected;
	callback.onConnectionFailed = &onConnectError;

	callback.onReceiveIMessage = &onReceiveIMessage;
	callback.onSendIMessage = &onSendIMessage;

	callback.onQueryUserState = &onQueryUserState;
	Ucs_init(&callback);	
	CUcPaasClient::Instance()->SetCallRingTone();
}

void FreeUCSNetServiceCallBackInterface()
{
	if (g_bInitUCSNet)
	{
		 Ucs_unInit();
	}
	g_bInitUCSNet = false;
}


CUcPaasClient* CUcPaasClient::m_lpInstance = NULL;
bool CUcPaasClient::ConnectUcPaas( const tstring strPhone )
{
	if (!ConnectUcPaasClientDemo(strPhone))
	{
		m_bIsUcPaasDemo = false;
		tstring strRootSid;
		tstring strRootToken;
		tstring strClientId;
		tstring strToken;
		if (!GetRootAccount(strRootSid, strRootToken))
		{
			CLogger::GetInstance()->PrintLog("get ucpaas root account failed.", LOG_ERROR);
			return false;
		}
		if (!GetUcpaasClientIdToken(strClientId, strToken))
		{
			CLogger::GetInstance()->PrintLog("get ucpaas client account from server failed.", LOG_ERROR);
			return false;			
		}

		Ucs_connect(strRootSid.c_str(), strRootToken.c_str(), strClientId.c_str(),  strToken.c_str());
		CLogger::GetInstance()->PrintLog(LOG_INFO, "Begin Connect Ucs");
		m_bIsConneting = true;
	}
	/*CVideoDlg::Instance()->ActiveVoiceDlg(false);*/
	CLogger::GetInstance()->PrintLog(LOG_INFO, "UCS_SDK_Version: %s", Ucs_getSDKVersion());
	return true;
}

bool CUcPaasClient::GetRootAccount(tstring& strSid, tstring& strToken)
{
	CDuiString strConfigFile = CFileHelper::GetCurrentAppPath();
	strConfigFile += UCPAAS_CFG_FILE;
	if (!CFileHelper::IsExistFile(strConfigFile))
	{
		return false;
	}

	std::ifstream  fFile(strConfigFile.GetData());
	if (!fFile.good())
	{
		return false;
	}

	Json::Value root;
	Json::Reader reader;
	try
	{
		if (reader.parse(fFile, root))
		{
			strSid = root["account"]["rootSid"].asString();
			strToken = root["account"]["rootToken"].asString();
			return true;
		}
	}	
	catch (...)
	{
		 CLogger::GetInstance()->PrintErrLog( "parse file exception: %s", strConfigFile.GetData());
		 return false;
	}
	
	return false;
}


bool CUcPaasClient::GetUcpaasClientIdToken(tstring& strClientId, tstring& strToken)
{
	CHttpClient httpClient;
	tstring strHttpResp;
	tstring strReq(WEB_SERVER_BASE_URL);
	strReq += "ucpaas/get-or-create-client";
	httpClient.Get(strReq.c_str(), CController::Instance()->GetToken(), strHttpResp);

	if (strHttpResp.find("clientNumber") != -1 && strHttpResp.find("clientPwd") != -1)
	{
		Json::Value root;
	Json::Reader reader;
	if (reader.parse(strHttpResp,root))
	{
		try
		{
			/*"balance": "0",
			"clientNumber": "61421041626042",
			"clientPwd": "ef5191bf",
			*/

			/*if (!root["resp"].isNull())
			{
				strClientId = root["resp"]["clientNumber"].asString();
				strToken = root["resp"]["clientPwd"].asString();
			}
			else
			{*/
				strClientId = root["clientNumber"].asString();
				strToken = root["clientPwd"].asString();
			//}	
		}catch(...){
			MessageBox(NULL,"获取认证失败","",IDOK);
			return false;
		}
	}
	}
	

	if (0 == strClientId.length() || 0 == strToken.length())
	{
		return false;
	}

	return true;
}



bool CUcPaasClient::ConnectUcPaasClientDemo(tstring strPhone)
{
	return false;
	CDuiString strConfigFile = CFileHelper::GetCurrentAppPath();
	strConfigFile += "config\\ucpaasDemoClint.cfg";
	if (!CFileHelper::IsExistFile(strConfigFile))
	{
		return false;
	}

	std::ifstream  fFile;//(strConfigFile.GetData());
	fFile.open(strConfigFile.GetData());
	Json::Value root;
	Json::Reader reader;
	if (reader.parse(fFile,root))
	{
		tstring strClientToken;
		tstring strClientId;
		tstring strRootID = root["rootID"].asString();
		tstring strRootToken = root["rootToken"].asString();
		for (int  idx = 0; idx < root["Clients"].size(); ++idx)
		{
			tstring strClientPhone = root["Clients"][idx]["phone"].asString();
			m_mapCallId.insert(make_pair( root["Clients"][idx]["clientID"].asString(),strClientPhone));
			if (0 == strPhone.compare(strClientPhone))
			{
				strClientId = root["Clients"][idx]["clientID"].asString();
				strClientToken =  root["Clients"][idx]["clientToken"].asString();
			}
		}

		if (strClientToken.length() > 0)
		{
			Ucs_connect(strRootID.c_str(), strRootToken.c_str(), strClientId.c_str(), strClientToken.c_str());
		}
	}

	return true;
}

CUcPaasClient::CUcPaasClient()
{
	m_bIsUcPaasDemo = true;
	m_bIsConneting = false;
}

bool CUcPaasClient::IsUcPaasContact( const tstring strContactPhone )
{
	/*上线后需server api来判断联系人是否注册ucpaas*/
	bool bIsUcpaasClient = false;
	if (m_bIsUcPaasDemo)
	{
		for (stringMapIter iter = m_mapCallId.begin(); iter != m_mapCallId.end(); ++iter)
		{
			if (0 == strContactPhone.compare(iter->second))
			{
				bIsUcpaasClient = true;
			}
		}		
	}
	else
	{
		bIsUcpaasClient = IsUcPaasClient(strContactPhone);
	}

	return bIsUcpaasClient;
}


bool CUcPaasClient::IsUcPaasClient(const tstring strContactPhone)
{
	tstring strUrl(WEB_SERVER_BASE_URL); 
	strUrl += "ucpaas/is-have-client?";
	strUrl += "cell_phone=";
	strUrl += strContactPhone;

	CHttpClient httpClient;
	tstring strHttpResp;
	httpClient.Get(strUrl, CController::Instance()->GetToken(), strHttpResp);

	if (strHttpResp.find("result") != -1)
	{
		Json::Value root;
		Json::Reader reader;
		if (reader.parse(strHttpResp,root))
		{
			return root["result"].asInt();
		}
	}
	
}

CUcPaasClient* CUcPaasClient::Instance()
{
	if (NULL == m_lpInstance)
	{
		m_lpInstance = new CUcPaasClient();
	}	
	return m_lpInstance;
}

std::tstring CUcPaasClient::GetPhoneFromCallId( const tstring strCallId )
{
	for (stringMapIter iter = m_mapCallId.begin(); iter != m_mapCallId.end(); ++iter)
	{
		if (0 == strCallId.compare(iter->first))
		{
			return iter->second;
		}
	}
	return "";
}

std::tstring CUcPaasClient::GetUcsErrInfo( int nErrCode )
{
	for (int idx = 0; idx < sizeof(g_stUcsErrCodeMap)/sizeof(stUcsErrorCodeMap); ++idx)
	{
		if (g_stUcsErrCodeMap[idx].uErrorCode == nErrCode)
		{
			return g_stUcsErrCodeMap[idx].strErrInfo;
		}
	}

	return _T("平台服务器内部错误 ");
}


bool CUcPaasClient::QueryUcsClientPhoneNo(const tstring& strClientId, tstring& strPhoneNo)
{
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += "ucpaas/get-cell-by-client?client_id=";
	strUrl += strClientId;

	CHttpClient httpClient;
	tstring strHttpResp;
	httpClient.Get(strUrl, CController::Instance()->GetToken(), strHttpResp);

	if (strHttpResp.find("cell") != -1)
	{
		Json::Value root;
		Json::Reader reader;
		if (reader.parse(strHttpResp,root))
		{
			try
			{
				if (!root["cell"].isNull())
				{
					strPhoneNo = root["cell"].asString();
					const int MOBILE_PHONE_LEN =11;
					if (strPhoneNo.length() > MOBILE_PHONE_LEN)
					{
						strPhoneNo = strPhoneNo.substr(0,MOBILE_PHONE_LEN -1);
					}
					return true;
				}			
			}catch(...){
				CLogger::GetInstance()->PrintErrLog("QueryUcsClientPhoneNo failed.httpResp=%s.", strHttpResp);
				return false;
			}
		}
	}
	
	return false;
}

bool CUcPaasClient::IsUcsConnected()
{
	return Ucs_isConnected();
}

bool CUcPaasClient::SetRingTone(const char *filePath)
{
	return Ucs_setRing(filePath) == 0;
}

bool CUcPaasClient::SetRingBackTone(const char *filePath)
{
	return Ucs_setRingBack(filePath) == 0;
}

bool CUcPaasClient::SetVolumeValue( unsigned int value )
{
	return Ucs_setVolumeValue(value);
}

 unsigned int CUcPaasClient::GetVolumeValue()
{
	return Ucs_getVolumeValue();
}

 void CUcPaasClient::SetMicMuteStatus( bool bMicMute )
 {
	 Ucs_setMicMuteStatus(bMicMute);
 }

 bool CUcPaasClient::CloseVideo( int mode )
 {
	return 0 == Ucs_closeVideo(mode);
 }

 bool CUcPaasClient::OpenVideo( int mode )
 {
	 return 0 == Ucs_openVideo(mode);
 }

 bool CUcPaasClient::AnswerCall( const char *callid )
 {
	 return 0 == Ucs_answer(callid);
 }

 bool CUcPaasClient::RejectCall( const char *callid )
 {
	 return 0 == Ucs_reject(callid);
 }

 void CUcPaasClient::SetCallRingTone()
 {
	 CDuiString strAppPath = CFileHelper::GetCurrentAppPath();
	 CDuiString strConfigFile = strAppPath + UCPAAS_CFG_FILE;	
	 std::ifstream  fFile(strConfigFile.GetData());
	
	 Json::Value root;
	 Json::Reader reader;
	 try
	 {
		 if (fFile.good() && reader.parse(fFile, root))
		 {
			 CDuiString strRingtone = strAppPath + "sound\\" + root["tone"]["ringtone"].asString().c_str();
			 CDuiString strRingbackTone = strAppPath + "sound\\" +  root["tone"]["ringbacktone"].asString().c_str();
			 
			 if (!SetRingBackTone(CChineseCode::EncodeUTF8(strRingbackTone.GetData()).c_str()))
			 {
				 CLogger::GetInstance()->PrintLog(LOG_DEBUG, "Set RingbackTone failed: %s", strRingbackTone.GetData());
			 }

			 if (!SetRingTone(CChineseCode::EncodeUTF8(strRingtone.GetData()).c_str()))
			 {
				 	CLogger::GetInstance()->PrintLog(LOG_DEBUG, "Set RingBackTone failed: %s", strRingtone.GetData());
			 }
		 }
	 }	
	 catch (...)
	 {
		 CLogger::GetInstance()->PrintErrLog( "parse file exception: %s", strConfigFile.GetData());
	 }
 }

 int CUcPaasClient::GetCameraNum()
 {
	 return Ucs_getCameraNum();
 }

 void CUcPaasClient::SendExCallStatus( const tstring strPhoneNo, const eExCallStatus eStatus )
 {
	  Ucs_sendInstanceMessage(strPhoneNo.c_str(), "", "", eStatus);
 }

