#include "StdAfx.h"
#include "voiceDlg.h"
#include "UCSNetServiceCallBackInterface.h"
#include "MsgBox.h"
#include "mainDlg.h"
#include "Utility.h"

#define DIAL_STATUS_DIALIING  "正在拨号......"
#define DIAL_STATUS_WAIT_ANSWER  "正在等待对方接听......"

const TCHAR* const kCloseDlgBtnName = _T("btnCloseVoiceDlg");

#define TIMER_UPDATE_CALL_TIMEOUT 1234
#define TIMER_UPDATE_CALL_TIME	 1235

#define TIME_SECOND 1000

CVoiceDlg* CVoiceDlg::m_spInstant = NULL;

CVoiceDlg::~CVoiceDlg(void)
{
	DeleteCriticalSection(&m_csStatu);	
}

CVoiceDlg * CVoiceDlg::Instance()
{
	if ( m_spInstant == NULL)
	{
		m_spInstant  = new CVoiceDlg();		
		m_spInstant->CreateWnd(mainDlg::getInstance()->GetHWND());
		m_spInstant->ActiveVoiceDlg(false);
	}	
	return m_spInstant;
}


void CVoiceDlg::SetCallTipInfo(const string &strTipsInfo)
{
	if ( NULL == m_pTxtCallInfo ) return ;
	m_strTipsInfo = strTipsInfo;
	m_pTxtCallInfo->SetText(strTipsInfo.c_str());
}

void CVoiceDlg::SetCallerInfo(const CDuiString &strNickName, const CDuiString &strOrgName,const CDuiString &strLogo)
{	
	m_pNickname->SetText(strNickName);
	m_pOrgNameLabel->SetText(strOrgName);

	if (!strLogo.IsEmpty())
	{
		m_pContactLogo->SetBkImage(strLogo);
	}
	

	if ( NULL != m_pSetTelVoice )
	{		
		m_pSetTelVoice->SetUserData("0");
		string strAtt = "normalimage=\"Call\\silence.png\" hotimage=\"Call\\silence.png\" pushedimage=\"Call\\silence.png\"";			
		m_pSetTelVoice->ApplyAttributeList(strAtt.c_str());		
	}
}


// 开始计时
bool CVoiceDlg::UpdateCallTime()
{
	SetCallStatus(CALL_CALLING);
	//SetUIStatus(false);
	m_uCallTime = 0;
	return m_PaintManager.SetTimer(m_pTxtCallInfo, TIMER_UPDATE_CALL_TIME, TIME_SECOND);		
}

// 结束计时
bool CVoiceDlg::StopUpdateCallTime()
{
	if ( NULL == m_pTxtCallInfo ) return false;
	m_PaintManager.KillTimer(m_pTxtCallInfo);	
	return true;
}

void CVoiceDlg::SetCallStatus(int nStatus)
{
	EnterCriticalSection(&m_csStatu);
	m_nCallStatus = nStatus;
	LeaveCriticalSection(&m_csStatu);
}

int CVoiceDlg::GetCallStatus()
{
	EnterCriticalSection(&m_csStatu);
	int iStatu = m_nCallStatus;
	LeaveCriticalSection(&m_csStatu);
	return iStatu;
}

void CVoiceDlg::OnBtnClickHangup(void)
{
	if (CALL_END == GetCallStatus())
	{
		return;
	}

	StopUpdateCallTime();
	SetCallStatus(CALL_END);
	Ucs_hangUp("");
	EnableHangUp(false);
	//ShowWindow(false);
	//mainDlg::getInstance()->EnableCallBtn(true);	
}


bool CVoiceDlg::OnSetVolume(bool bMicMute, unsigned nVolumeMic, unsigned nVolumeSound)
{
	Ucs_setMicMuteStatus(bMicMute);
	return true;
}

void CVoiceDlg::OnCloseClick()
{
	int nCallStatus = GetCallStatus();
	switch (nCallStatus)
	{
	case CALL_DIALLING:				
	case CALL_CALLING:
		{
			string strTip;
			if ( CALL_DIALLING == nCallStatus )
			{
				strTip = "当前正在呼叫中，是否挂断？";
			}
			else
			{	
				strTip = "当前正在通话中，是否挂断？";
			}
			CMsgBox* pMsgBox = new CMsgBox("",strTip,MSGBOX_ICON_ASK, MSGBOX_IDYESNO);
			if (MSGBOX_IDYES == pMsgBox->ShowModal(NULL))
			{
				OnBtnClickHangup();				
			}		
			else
			{
				return;
			}
			break;
		}
	default:
		{	
		}
	}

	//mainDlg::getInstance()->EnableCallBtn(true);	
	//StopUpdateCallTime();
	ActiveVoiceDlg(false);	
}

LRESULT CVoiceDlg::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
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
		default:
			break;
		}
	}
	return 0;
}

void CVoiceDlg::InitWindow()
{
	m_pTxtCallInfo = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("callStatus")));
	m_pNickname   = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("nickname")));
	m_pOrgNameLabel    = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("orgname")));

	m_pSetTelVoice = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnSetVoice")));
	m_pContactLogo = static_cast<CControlUI*>(m_PaintManager.FindControl(_T("logo")));
	m_pBtnHangup = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("btnHangup")));
	EnableHangUp(false);
}

DuiLib::CDuiString CVoiceDlg::GetSkinFile()
{
	return _T("Call\\voiceFrmUI.xml");
}

void CVoiceDlg::HandleHangupEvent( int reason )
{
		StopUpdateCallTime();
		SetCallStatus(CALL_END);	
		SetCallTipInfo(CUcPaasClient::GetUcsErrInfo(reason));	
		EnableHangUp(false);
		ActiveVoiceDlg(false);
		//mainDlg::getInstance()->EnableCallBtn(true);
}

void CVoiceDlg::HandleAnswerEvent()
{
	UpdateCallTime();
	SetCallStatus(CALL_CALLING);
	if (VOICE_CALL_LANDING == m_ucsDialType)
	{
		m_PaintManager.KillTimer(m_pBtnHangup);
	}
}

void CVoiceDlg::HandleDialFailedEvent( int reason )
{
	SetCallStatus(CALL_NORMAL);
	CMsgBox* pMsgBox = new CMsgBox("",CUcPaasClient::GetUcsErrInfo(reason));
	pMsgBox->ShowModal(NULL);
	ActiveVoiceDlg(false);
}

void CVoiceDlg::HandleAlertingEvent()
{
	if (CALL_END == GetCallStatus())
	{
		return;
	}
	EnableHangUp(true);
	SetCallStatus(CALL_DIALLING);
	SetCallTipInfo(DIAL_STATUS_WAIT_ANSWER);
}

void CVoiceDlg::EnableHangUp(bool bEnabled)
{
	m_pBtnHangup->SetEnabled(bEnabled);
	if (bEnabled)
	{		
		m_pBtnHangup->SetNormalImage("Call\\hangup2.PNG");
	}
	else
	{
		m_pBtnHangup->SetNormalImage("Call\\hangup1.PNG");
	}
}


void CVoiceDlg::HandleClickEvent( TNotifyUI& msg )
{
	if ( msg.pSender == m_pBtnHangup)
	{
		OnBtnClickHangup();			
	}	
	else if (msg.pSender->GetName() == kCloseDlgBtnName)
	{
		OnCloseClick();
	}
	else if ( msg.pSender == m_pSetTelVoice )
	{
		const bool bIsMicMute = ( m_pSetTelVoice->GetUserData() == "0" ) ? ( true ) : ( false );
		if ( !OnSetVolume(bIsMicMute, 100, 100) )
		{
			return;
		}

		string strAtt;
		if ( bIsMicMute )
		{
			m_pSetTelVoice->SetUserData("1");
			strAtt = "normalimage=\"Call\\silence.png\" hotimage=\"Call\\silence.png\" pushedimage=\"Call\\silence.png\"";					
		}	
		else
		{
			m_pSetTelVoice->SetUserData("0");
			strAtt = "normalimage=\"Call\\silence1.png\" hotimage=\"Call\\silence1.png\" pushedimage=\"Call\\silence1.png\"";					
		}	

		m_pSetTelVoice->ApplyAttributeList(strAtt.c_str());
	}
}

void CVoiceDlg::HandleTimerEvent( TNotifyUI& msg )
{
	if (msg.pSender == m_pBtnHangup)
	{
		m_pTxtCallInfo->SetText("对方无应答，拨号超时");	
		Ucs_hangUp("");
		m_PaintManager.KillTimer(m_pBtnHangup);
	}

	if ( msg.pSender == m_pTxtCallInfo )
	{	
		m_uCallTime++;
		int nHour = m_uCallTime / 3600;			
		int nSec  = m_uCallTime;					

		char szTime[256] = {0};
		if ( nHour > 0 )
		{
			int nMins = (m_uCallTime - nHour * 3600 ) / 60;
			nSec  = m_uCallTime - nHour * 3600 - nMins * 60;
			sprintf(szTime, "通话时间  %02d:%02d:%02d",  nHour, nMins, nSec);
		}
		else 
		{
			int nMins = m_uCallTime / 60;
			if ( nMins > 0 )
			{
				nSec = m_uCallTime - nMins * 60;
			}
			sprintf(szTime, "通话时间 %02d:%02d",  nMins, nSec);
		}
		m_pTxtCallInfo->SetText(szTime);	
	}	

}

void CVoiceDlg::LauchCall(UCS_DIALTYPE eDialType)
{
	m_ucsDialType = eDialType;
	SetCallerInfo(m_strNickname,  m_strOrgName, m_strAvatar);
	SetCallTipInfo(DIAL_STATUS_DIALIING);
	while (!CUcPaasClient::Instance()->IsUcsConnected())
	{
		Sleep(1000);
	}
	Ucs_dial(m_ucsDialType, m_strPhone.GetData());	
	SetCallStatus(CALL_LAUCH_DIAL);	
	if (VOICE_CALL_LANDING == m_ucsDialType)
	{
		m_PaintManager.SetTimer(m_pBtnHangup, TIMER_UPDATE_CALL_TIMEOUT, TIME_SECOND*30);		
	}
}

void CVoiceDlg::InitalDialInfo(CDuiString strPhone, CDuiString strNickname,CDuiString strOrgName,CDuiString strAvatar)
{
	m_strPhone = strPhone;
	m_strNickname = strNickname;
	m_strOrgName = strOrgName;
	m_strAvatar = strAvatar;
}


void CVoiceDlg::ActiveVoiceDlg(bool bActive)
{
	ShowWindow(bActive);
	m_bIsActive = bActive;
	if (!bActive)
	{
		m_pTxtCallInfo->SetText("");
		m_pNickname->SetText("");
		m_pOrgNameLabel->SetText("");
	}	
}

bool CVoiceDlg::IsActive()
{
	return m_bIsActive;
}

CVoiceDlg::CVoiceDlg( void )
{
	m_pTxtCallInfo = NULL;
	m_pSetTelVoice = NULL;
	m_pContactLogo  = NULL;
	m_pNickname = NULL;
	m_pOrgNameLabel = NULL;
	m_nCallStatus = CALL_NORMAL;
	m_uCallTime   = 0;
	m_ucsDialType = VOICE_CALL;

	InitializeCriticalSection(&m_csStatu);
}

