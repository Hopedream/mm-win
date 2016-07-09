#include "StdAfx.h"
#include "videoDlg.h"
#include "MsgBox.h"
#include "mainDlg.h"
#include "Utility.h"
#include "volumeUI.h"
#include "Controller.h"
#include "UserManager.h"
#include "../include/DBTaskModule.h"
#include "../Common/Logger.h"
#include "../include/Global_Def.h"
#include "AudioCallIncomingDlg.h"
#include "TrayTaskmoduleimpl.h"
#define VOICE_PAGE_VIEW 0
#define VIDEO_PAGE_VIEW 1

#define DIAL_STATUS_DIALIING  "正在拨号......"
#define DIAL_STATUS_WAIT_ANSWER  "正在等待对方接听......"

const TCHAR* const kCloseDlgBtnName = _T("btnCloseVoiceDlg");


#define TIMER_UPDATE_CALL_TIME	 1235

#define TIME_SECOND 1000

CVideoDlg* CVideoDlg::m_spInstant = NULL;

CVideoDlg::~CVideoDlg(void)
{
	DeleteCriticalSection(&m_csStatu);	
}


CVideoDlg * CVideoDlg::Instance()
{
	if ( m_spInstant == NULL)
	{
		m_spInstant  = new CVideoDlg();		
		m_spInstant->CreateWnd(/*mainDlg::getInstance()->GetHWND()*/);
		m_spInstant->ActiveVoiceDlg(false);
		CController::Instance()->GetUserInfoMgr()->AddEventHandler((IUserManagerEvent*)m_spInstant);
		GetTrayTaskModule()->AddEventHandler((ITrayTaskModuleEvent*)m_spInstant);
	}	
	return m_spInstant;
}


void CVideoDlg::ShowVideoWnd(bool bVisible)
{
	::ShowWindow(m_remoteVideo, bVisible&&m_remoteVideo);	
	::ShowWindow(m_localVideo, bVisible);	
	//if (bVisible)
	//{
		Ucs_setVideoHandle(m_localVideo, m_remoteVideo);	
	//}
}


void CVideoDlg::SetCallTipInfo(const tstring strTipsInfo)
{
	if (m_bVideoView)
	{
		m_pVideoCallStatus->SetText(strTipsInfo.c_str());
	}
	else 
	{	
		m_pVoiceCallStatus->SetText(strTipsInfo.c_str());
	}
}


// 开始计时
bool CVideoDlg::UpdateCallTime()
{
	m_uCallTime = 0;
	return m_PaintManager.SetTimer(m_pCallTime, TIMER_UPDATE_CALL_TIME, TIME_SECOND);		
}


// 结束计时
bool CVideoDlg::StopUpdateCallTime()
{
	if ( NULL == m_pCallTime ) return false;
	m_PaintManager.KillTimer(m_pCallTime);	
	return true;
}

void CVideoDlg::SetCallStatus(int nStatus)
{
	EnterCriticalSection(&m_csStatu);
	m_nCallStatus = nStatus;
	LeaveCriticalSection(&m_csStatu);
}

int CVideoDlg::GetCallStatus()
{
	EnterCriticalSection(&m_csStatu);
	int iStatu = m_nCallStatus;
	LeaveCriticalSection(&m_csStatu);
	return iStatu;
}



void CVideoDlg::OnBtnClickHangup(void)
{
	if (CALL_END == GetCallStatus())
	{
		return;
	}

	StopUpdateCallTime();

	SetCallStatus(CALL_END);
	Ucs_hangUp("");
	EnableHangUp(false);
	ActiveVoiceDlg(false);
	//mainDlg::getInstance()->EnableCallBtn(true);	
}



void CVideoDlg::OnCloseClick()
{
	int nCallStatus = GetCallStatus();

	if (CALL_DIALLING == nCallStatus || CALL_CALLING == nCallStatus || CALL_LAUCH_DIAL == nCallStatus)
	{
		tstring strTip;
		if ( CALL_DIALLING == nCallStatus )
		{
			strTip = "当前正在呼叫中，是否挂断？";
		}
		else
		{	
			strTip = "当前正在通话中，是否挂断？";
		}
		CMsgBox* pMsgBox = new CMsgBox("",strTip,MSGBOX_ICON_ASK, MSGBOX_IDYESNO);
		if (MSGBOX_IDNO == pMsgBox->ShowModal(NULL))
		{			
			return;
		}		
		OnBtnClickHangup();		
	}	
	
	ActiveVoiceDlg(false);
}

LRESULT CVideoDlg::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{	
	if (WM_UCPAAS_NOTIFY == uMsg)
	{
		switch(wParam)
		{
		case eHangup:
			HandleHangupEvent(lParam);
			break;
		case eAnswer:
			HandleAnswerEvent();
			break;
		case eDialFailed:
			HandleDialFailedEvent(lParam);
			break;
		case  eAlerting:
			HandleAlertingEvent();
			break;
		case eExStatus:
			HandleExStatus((eExCallStatus)lParam);
			break;
		case eConnectSuccess:					
			HandleUcsConnSuccessEvent();
			break;
		case eConnectFailed:
			HandleUcsConnFailedEvent(lParam);
			break;	
		case eCallback:
			HandleCallBackEvent(lParam);		
			break;
		case eIncomingCall:
			HandleIncomming((stUcsIncommingCall*)lParam);
			break;
		default:
			break;
		}
	}	
	else if (WM_CALL_IP_PHONE == uMsg || WM_CALL_SMART_PHONE == uMsg || WM_CALL_BACK == uMsg )
	{
		OnLauchNetCall(mainDlg::getInstance()->getCurItem().m_strAccount, uMsg);
	}	
	return 0;
}


void CVideoDlg::InitWindow()
{
	m_pVideoCallStatus = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("txtCallStatus")));
	m_pCallTime = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("callTime")));
	m_pFrmCaption = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("frmCaption")));	
	m_pSetTelVoice = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnSetVoice")));
	m_pBtnHangup = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnHangup")));

	m_pBtnEnterCurChatPage = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnGotoCurChatPage")));
	m_pBtnVolume = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnVolume")));
	m_pBtnCamera = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnSetCamera")));
	m_pPinTopWnd = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnTopWnd")));	
	m_pTabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl(_T("switchLayout")));		
	
	m_pVoiceCallStatus = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("callStatus")));
	m_pNickname   = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("nickname")));
	m_pOrgNameLabel    = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("orgname")));
	m_pContactLogo = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("logo")));
	m_pCaneraInfo = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("CameraInfo")));

	EnableHangUp(false);

	RECT rc;
	::GetClientRect(*this, &rc);
	CHorizontalLayoutUI* lpHLayoutUI = static_cast<CHorizontalLayoutUI*>(m_PaintManager.FindControl(_T("myViedo")));
	
	m_remoteVideo  = CreateWindow(_T("STATIC"), _T(""), WS_VISIBLE | WS_CHILD , rc.left, rc.top+30, (rc.right - rc.left), (rc.bottom-30-50), m_PaintManager.GetPaintWindow(), NULL, NULL, NULL);
	m_localVideo = CreateWindow(_T("STATIC"), _T(""), WS_VISIBLE | WS_CHILD, 410, 310, 120, 90, m_PaintManager.GetPaintWindow(), NULL, NULL, NULL);
	
	ShowVideoWnd(false);
	SetMicMuteStatus(false);
	SetLocalVideoStatus(true);
	SetWndPinStatus(false);
}

DuiLib::CDuiString CVideoDlg::GetSkinFile()
{
	return _T("Call\\videoFrmUI.xml");
}

void CVideoDlg::HandleHangupEvent( int reason )
{
	if (m_bVideoView)
	{
		ShowVideoWnd(false);
	}	
	StopUpdateCallTime();

	SetCallStatus(CALL_END);	
	SetCallTipInfo(CUcPaasClient::GetUcsErrInfo(reason));	
	EnableHangUp(false);
	ActiveVoiceDlg(false);
	//mainDlg::getInstance()->EnableCallBtn(true);
}

void CVideoDlg::HandleAnswerEvent()
{	
	if (CALL_END == GetCallStatus())
	{
		Ucs_hangUp(""); //收到alert之前主叫方挂电话不起作用，再次挂掉
		return;
	}

	UpdateCallTime();
	SetCallStatus(CALL_CALLING);	
	SetCallTipInfo("正在通话中......");
	if (m_bVideoView)
	{
		ShowVideoWnd(true);
	}
}

void CVideoDlg::HandleDialFailedEvent( int reason )
{
	CLogger::GetInstance()->PrintErrLog("Dial failed, ErrCode:%d, description: %s", reason, CUcPaasClient::GetUcsErrInfo(reason));
	SetCallStatus(CALL_NORMAL);
	CMsgBox* pMsgBox = new CMsgBox("拨号失败",CUcPaasClient::GetUcsErrInfo(reason));
	pMsgBox->ShowModal();
	ActiveVoiceDlg(false);
}

void CVideoDlg::HandleAlertingEvent()
{
	if (CALL_END == GetCallStatus())
	{
		Ucs_hangUp(""); //收到alert之前主叫方挂电话不起作用，再次挂掉
		return;
	}
	EnableHangUp(true);
	SetCallStatus(CALL_DIALLING);
	SetCallTipInfo(DIAL_STATUS_WAIT_ANSWER);
}

void CVideoDlg::EnableHangUp(bool bEnabled)
{
	//m_pBtnHangup->SetEnabled(bEnabled);
	if (bEnabled)
	{		
		//m_pBtnHangup->SetNormalImage("Call\\hangup2.PNG");
	}
	else
	{
		//m_pBtnHangup->SetNormalImage("Call\\hangup1.PNG");
	}
}


void CVideoDlg::HandleClickEvent( TNotifyUI& msg )
{
	if ( msg.pSender == m_pBtnHangup)
	{
		OnBtnClickHangup();			
	}	
	else if (msg.pSender->GetName() == kCloseDlgBtnName)
	{
		OnCloseClick();
	}
	else if (msg.pSender == m_pBtnEnterCurChatPage)
	{
		tstring strAccount = m_strPhone + MMPC_ACCOUNT;
		mainDlg::getInstance()->OnShowChatWnd(strAccount);
		//mainDlg::getInstance()->OnSetFocus()
		SetFocus(mainDlg::getInstance()->GetHWND());
	}
	else if (msg.pSender == m_pBtnVolume)
	{
		RECT rect = m_pBtnVolume->GetRelativePos();
		CVolumeUI *pMenu = new CVolumeUI();
		pMenu->Init(*this, msg.ptMouse);
		pMenu->ShowWindow(TRUE);
	}
	else if ( msg.pSender == m_pSetTelVoice )
	{
		SetMicMuteStatus( m_pSetTelVoice->GetUserData() == "0");
	}
	else if ( msg.pSender == m_pBtnCamera )
	{
		SetLocalVideoStatus( m_pBtnCamera->GetUserData() == "0");
	}
	else if ( msg.pSender == m_pPinTopWnd )
	{
		SetWndPinStatus( m_pPinTopWnd->GetUserData() == "0");
	}
}

void CVideoDlg::SetMicMuteStatus(bool bIsMicMute)
{
	CUcPaasClient::Instance()->SetMicMuteStatus(bIsMicMute);
	CDuiString strAtt;
	if (bIsMicMute)
	{
		m_pSetTelVoice->SetUserData("1");
		strAtt = "normalimage=\"Call\\mic_blue.PNG\" hotimage=\"Call\\mic_grey.png\" pushedimage=\"Call\\mic_grey.png\"";				
	}
	else 
	{
		m_pSetTelVoice->SetUserData("0");
		strAtt = "normalimage=\"Call\\mic_grey.png\" hotimage=\"Call\\mic_blue.png\" pushedimage=\"Call\\mic_blue.png\"";				
	}
	m_pSetTelVoice->ApplyAttributeList(strAtt);
}

void CVideoDlg::SetLocalVideoStatus(bool bIsOpened)
{
	CDuiString strAtt;
	if (bIsOpened)
	{
		CUcPaasClient::Instance()->OpenVideo(VIDEO_SEND_MODE);
		m_pBtnCamera->SetUserData("1");
		strAtt = "normalimage=\"Call\\cam_blue.PNG\" hotimage=\"Call\\cam_grey.png\" pushedimage=\"Call\\cam_grey.png\"";				
	}
	else 
	{
		CUcPaasClient::Instance()->CloseVideo(VIDEO_SEND_MODE);
		m_pBtnCamera->SetUserData("0");
		strAtt = "normalimage=\"Call\\cam_grey.png\" hotimage=\"Call\\cam_blue.png\" pushedimage=\"Call\\cam_blue.png\"";				
	}
	m_pBtnCamera->ApplyAttributeList(strAtt);
}

void CVideoDlg::SetWndPinStatus(bool bIsTop)
{
	CDuiString strAtt;
	if (bIsTop)
	{
		SetTopMostWnd(true);
		m_pPinTopWnd->SetUserData("1");
		strAtt = "normalimage=\"Call\\pin_top.PNG\"";				
	}
	else 
	{
		SetTopMostWnd(false);
		m_pPinTopWnd->SetUserData("0");
		strAtt = "normalimage=\"Call\\pin_default.png\"";				
	}
	m_pPinTopWnd->ApplyAttributeList(strAtt);
}



void CVideoDlg::HandleTimerEvent( TNotifyUI& msg )
{
	if ( msg.pSender == m_pCallTime )
	{	
		m_uCallTime++;
		int nHour = m_uCallTime / 3600;			
		int nSec  = m_uCallTime;					

		char szTime[256] = {0};
		if ( nHour > 0 )
		{
			int nMins = (m_uCallTime - nHour * 3600 ) / 60;
			nSec  = m_uCallTime - nHour * 3600 - nMins * 60;
			sprintf(szTime, " %02d:%02d:%02d",  nHour, nMins, nSec);
		}
		else 
		{
			int nMins = m_uCallTime / 60;
			if ( nMins > 0 )
			{
				nSec = m_uCallTime - nMins * 60;
			}
			sprintf(szTime, " %02d:%02d",  nMins, nSec);
		}
		m_pCallTime->SetText(szTime);	
	}	
}


void CVideoDlg::SetCallerInfo()
{	
	CDuiString strCaption;
	if (m_bVideoView)
	{
		if (CUcPaasClient::Instance()->GetCameraNum() == 0)
		{
			CLogger::GetInstance()->PrintLog(LOG_INFO, "local camera don't exist.");
			CUcPaasClient::Instance()->SendExCallStatus(m_strPhone.GetData(), eLackCamera);
		}
		CUcPaasClient::Instance()->OpenVideo(VIDEO_RECEIVE_MODE);		
		strCaption.Format("正在和%s进行视频通话", m_strNickname.GetData());				
	}
	else
	{
		m_pNickname->SetText(m_strNickname);
		m_pOrgNameLabel->SetText(m_strOrgName);
		strCaption.Format("正在和%s进行语音通话", m_strNickname.GetData());
		if (!m_strAvatar.IsEmpty())
		{
			m_pContactLogo->SetBkImage(m_strAvatar);
		}
	}

	m_pBtnCamera->SetVisible(m_bVideoView);

	CDuiString strCallStatusMsg;
	strCallStatusMsg.Format("正在呼叫 %s", m_strNickname.GetData());
	SetCallTipInfo(strCallStatusMsg.GetData());
	m_pFrmCaption->SetText(strCaption);

}

bool CVideoDlg::LauchCall(const UCS_DIALTYPE eDialType)
{
	int nPinCount=0;
	while (!CUcPaasClient::Instance()->IsUcsConnected())
	{		
		nPinCount++;
		if (nPinCount == 5)
		{
			return false;
		}

		CLogger::GetInstance()->PrintLog(LOG_INFO, "Waiting connect ucs.....");
		Sleep(1000);
	}

	SetVideoView(VIDEO_CALL == eDialType);

	m_ucsDialType = eDialType;
	SetCallerInfo();
	
	Ucs_dial(eDialType, m_strPhone.GetData());
	SetCallStatus(CALL_LAUCH_DIAL);	

	/*if (VOICE_CALL_LANDING == m_ucsDialType)
	{
		m_PaintManager.SetTimer(m_pBtnHangup, TIMER_UPDATE_CALL_TIMEOUT, TIME_SECOND*30);		
	}*/

	return true;
}

void CVideoDlg::InitalDialInfo(	CDuiString strPhone, CDuiString strNickname,CDuiString strOrgName,CDuiString strAvatar)
{
	m_strPhone = strPhone;
	m_strNickname = strNickname;
	m_strOrgName = strOrgName;
	m_strAvatar = strAvatar;
}

void CVideoDlg::ActiveVoiceDlg(bool bActive)
{
	ShowVideoWnd(false);
	ShowWindow(bActive, true);
	m_bIsActive = bActive;	
	if (!bActive)
	{
		SetCallTipInfo("");
		m_pCallTime->SetText("");
		m_pCaneraInfo->SetText("");
		SetMicMuteStatus(false);
		SetLocalVideoStatus(true);
	}	
	else
	{
		SetForegroundWnd();
	}
}

bool CVideoDlg::IsActive()
{
	return m_bIsActive;
}

CVideoDlg::CVideoDlg( void )
{
	m_pSetTelVoice = NULL;
	m_nCallStatus = CALL_NORMAL;
	m_uCallTime   = 0;
	m_bShowRemoteVideo = true;	
	InitializeCriticalSection(&m_csStatu);
}


void CVideoDlg::SetVideoView(bool bVideoView)
{
	m_bVideoView = bVideoView;
	m_pTabLayout->SelectItem(bVideoView ? VIDEO_PAGE_VIEW: VOICE_PAGE_VIEW);
	m_pBtnCamera->SetVisible(bVideoView);
}


void CVideoDlg::HandleExStatus(eExCallStatus eStatus )
{
	if (eLackCamera == eStatus)
	{
		CLogger::GetInstance()->PrintLog(LOG_INFO, "remote camera don't exist.");
		m_pCaneraInfo->SetText("对方没有打开视频设备");
		m_bShowRemoteVideo = false;
		::ShowWindow(m_remoteVideo, false);	
	}
}



void CVideoDlg::HandleUcsConnSuccessEvent()
{
	CLogger::GetInstance()->PrintLog(LOG_INFO, "Connect Ucs sucess.");		
}

void CVideoDlg::HandleCallBackEvent(int reason)
{
	tstring strMsg;
	if (0 == reason)
	{
		strMsg = "双向回拨请求成功";			
	}
	else
	{
		strMsg = CUcPaasClient::GetUcsErrInfo(reason);
	}
	CMsgBox* pMsgBox = new CMsgBox("", strMsg);
	pMsgBox->ShowModal();
}


void CVideoDlg::HandleUcsConnFailedEvent(int reason)
{
#ifdef _DEBUG
	//fiCMsgBox::MsgBox(NULL, "云之讯", CUcPaasClient::GetUcsErrInfo(reason), MSGBOX_ICON_ERROR);
#endif
	CLogger::GetInstance()->PrintErrLog("Connect ucpaas filed, errCode: %d, desription: %s.  retry again...", reason, CUcPaasClient::GetUcsErrInfo(reason));
	CUcPaasClient::Instance()->ConnectUcPaas();
}


void CVideoDlg::OnLauchNetCall(const CDuiString strAccount, int type)
{
	if ( !CUcPaasClient::Instance()->IsConnecting())
	{
		CMsgBox* pMsgBox = new CMsgBox("", "链接视频语音网络平台失败，请检查网络", MSGBOX_ICON_ERROR);
		pMsgBox->ShowModal();
		return;
	}

	if (IsActive())
	{
		ShowWindow(SW_SHOWDEFAULT);
		return;
	}

	UCS_DIALTYPE eDialType;
	EUsrState eState = CController::Instance()->GetUserInfoMgr()->GetState(strAccount.GetData());

	switch(type)
	{
	case WM_CALL_SMART_PHONE:
		eDialType = VOICE_CALL_LANDING;
		break;
	case WM_VIDEO_CALL:
		if (eState == enmState_Offline)
		{
			CMsgBox::MsgBox(mainDlg::getInstance()->GetHWND(), "拨号失败","当前用户不在线，无法进行网络视频通信", MSGBOX_ICON_WARNING);
			return;
		}
		eDialType = VIDEO_CALL;
		break;
	case WM_CALL_BACK:
		Ucs_callBack(ExtractPhoneNo(strAccount));
		return;
	case WM_CALL_IP_PHONE:
		{
			if (eState == enmState_Offline)
			{
				CMsgBox* lpMsgBox = new CMsgBox("拨号失败","当前用户不在线，无法进行网络语音通信", MSGBOX_ICON_WARNING);
				lpMsgBox->ShowModal(mainDlg::getInstance()->GetHWND());
				return;
			}
			eDialType = VOICE_CALL;
		}
		break;
	case WM_VOICE_AUTO:
		eDialType = (eState == enmState_Online ? VOICE_CALL: VOICE_CALL_LANDING);
		break;
	default:
		break;
	}

	sFriendListItem friendItem;
	if (!mainDlg::getInstance()->GetCallInFriendItem(ExtractPhoneNo(strAccount).GetData(), friendItem))
	{
		CMsgBox* pMsgBox = new CMsgBox("","获取联系人信息失败", MSGBOX_ICON_ERROR);
		pMsgBox->ShowModal();
	}


	CVideoDlg::Instance()->InitalDialInfo( ExtractPhoneNo(strAccount), friendItem.strShowName.c_str(), "", friendItem.strAvatar.c_str());		
	if(CVideoDlg::Instance()->LauchCall(eDialType))
	{
		CVideoDlg::Instance()->ActiveVoiceDlg(true);
		GetDBTaskModule()->SaveCallHistoryList(strAccount.GetData(), eDialCallOut);
	}
	else
	{
		CMsgBox* pMsgBox = new CMsgBox("连接超时","连接云之讯服务器失败！", MSGBOX_ICON_ERROR);
		pMsgBox->ShowModal();
	}
	
}

void CVideoDlg::Destroy()
{
	if (CALL_END != m_spInstant->GetCallStatus())
	{
		Ucs_hangUp(""); 
	}
	CController::Instance()->GetUserInfoMgr()->RemoveEventHandler((IUserManagerEvent*)m_spInstant);
	delete m_spInstant;
}

void CVideoDlg::OnUserStateUpdated( tstring& strAccount, EUsrState eState, tstring strRes, tstring strBusyInfo /*= ""*/ )
{
	if (m_bIsActive && tstring::npos != strAccount.find(m_strPhone) && enmState_Offline == eState)
	{
		CMsgBox::MsgBox(GetHWND(), "下线通知", "对方已经下线，通话终止", MSGBOX_ICON_WARNING);
		Ucs_hangUp("");
		ActiveVoiceDlg(false);
	}
}

void CVideoDlg::HandleIncomming(stUcsIncommingCall* lpUcsCallInfo)
{	
	sFriendListItem userInfo;
	tstring strPhoneNo;
	if (!CUcPaasClient::Instance()->QueryUcsClientPhoneNo(lpUcsCallInfo->strCallerNumber, strPhoneNo))
	{
		CLogger::GetInstance()->PrintErrLog("QueryUcsClientPhoneNo failed. CallerNumber:%s", lpUcsCallInfo->strCallerNumber.c_str());
		return;
	}

	if (!mainDlg::getInstance()->GetCallInFriendItem(strPhoneNo, userInfo))
	{
		CLogger::GetInstance()->PrintErrLog("Query user info failed. PhoneNo:%s", strPhoneNo.c_str());
		return;
	}

	GetTrayTaskModule()->Flash(userInfo.strAccount, enmMsgType_incomeCall);

	m_pIncomeDlg = new CUcsIncomeingDlg();
	m_pIncomeDlg->CreateCallInDlg();
	m_pIncomeDlg->InitalCallInDlg(userInfo.strShowName, lpUcsCallInfo->eCallType == VIDEO_CALL);
	int retCode = m_pIncomeDlg->ShowModal();
	GetTrayTaskModule()->StopFlash(userInfo.strAccount, enmMsgType_incomeCall);
	if ( CALL_HANGUP ==  retCode) 
	{
		CUcPaasClient::Instance()->RejectCall(lpUcsCallInfo->strCallid.c_str());
		GetDBTaskModule()->SaveCallHistoryList(userInfo.strAccount, eCallBarring);
	}		
	else if (CALL_ANSER == retCode)
	{
		GetDBTaskModule()->SaveCallHistoryList(userInfo.strAccount, eDialCallIn);
		if (CUcPaasClient::Instance()->GetCameraNum() == 0)
		{
			CLogger::GetInstance()->PrintLog(LOG_INFO, "local camera don't exist.");
			CUcPaasClient::Instance()->SendExCallStatus(strPhoneNo, eLackCamera);
		}

		CUcPaasClient::Instance()->AnswerCall("");
		SetCallStatus(CALL_CALLING);
		ActiveVoiceDlg(true);
		SetVideoView(lpUcsCallInfo->eCallType == VIDEO_CALL);

		InitalDialInfo(strPhoneNo.c_str(), userInfo.strShowName.c_str(), "", userInfo.strAvatar.c_str());	
		SetCallerInfo();
		CDuiString strCallStatus;
		strCallStatus.Format("正在和 %s 进行通话......", userInfo.strShowName.c_str());
		SetCallTipInfo(strCallStatus.GetData());
		if (lpUcsCallInfo->eCallType == VIDEO_CALL)
		{
			CUcPaasClient::Instance()->OpenVideo(VIDEO_RECEIVE_MODE);
			ShowVideoWnd(true);		
		}
		EnableHangUp(true);
	}	

	delete lpUcsCallInfo;
}

void CVideoDlg::OnDbClickCallIncomeMessage( tstring strNewUser )
{
	m_pIncomeDlg->CenterWindow();
}

