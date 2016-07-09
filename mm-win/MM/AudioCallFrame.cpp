#include "StdAfx.h"
#include "AudioCallFrame.h"
#include "ucpaas/UCSClient.h"
#include "UCSNetServiceCallBackInterface.h"
#include "MsgBox.h"
#include "mainDlg.h"



#define TIMER_UPDATE_CALL_STATUS 1234
#define TIMER_UPDATE_CALL_TIME	 1235

CUcsCallFrame * CUcsCallFrame::m_spInstant = NULL;


CUcsCallFrame::CUcsCallFrame(void)
{
	m_spInstant    = NULL;	
	m_pTxtMyName    = NULL;
	m_pTxtCallInfo = NULL;
	m_pBnDialboard = NULL;
	m_pCallStatus  = NULL;
	m_pSetTelVoice = NULL;
	m_pMyselfImage    = NULL;
	m_pContactImage  = NULL;
	m_pVerCallPanel   = NULL;
	m_nCallStatus = CALL_NORMAL;
	m_uCallTime   = 0;
	m_nEditCurPos = 0;
	m_bVideoFlag  = false;
	InitializeCriticalSection(&m_csStatu);
	m_nStatusImgIdx = 0;

}

CUcsCallFrame::~CUcsCallFrame(void)
{
	DeleteCriticalSection(&m_csStatu);
	if (NULL != m_spInstant)
	{
		delete m_spInstant;
		m_spInstant = NULL;
	}	
}

CUcsCallFrame * CUcsCallFrame::Instance()
{
	if ( m_spInstant == NULL)
	{
		m_spInstant  = new CUcsCallFrame();	
		m_spInstant->Create(NULL, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);		
		m_spInstant->CenterWindow();	
	}	
	return m_spInstant;
}


int CUcsCallFrame::SetVideoWindow()
{
	//Sleep(1000);
	RECT rc;
	::GetClientRect(*this, &rc);
	::SetWindowPos(m_localVideo, NULL, rc.right / 2 - 340 - 40, 65, 320, 240, SWP_NOZORDER | SWP_NOACTIVATE);
	::SetWindowPos(m_remoteVideo, NULL, rc.right / 2 + 60, 65, 320, 240, SWP_NOZORDER | SWP_NOACTIVATE);
	int iRet = Ucs_setVideoHandle(m_localVideo, m_remoteVideo);
	return iRet;
}

void CUcsCallFrame::ShowVideo(bool enable)
{
	if(enable)
	{
		m_pMyselfImage->SetVisible(false);
		m_pContactImage->SetVisible(false);
		::ShowWindow(m_localVideo, SW_SHOW);
		::ShowWindow(m_remoteVideo, SW_SHOW);
		SetVideoWindow();
	}
	else
	{
		m_pMyselfImage->SetVisible(true);
		m_pContactImage->SetVisible(true);
		::ShowWindow(m_localVideo, SW_HIDE);
		::ShowWindow(m_remoteVideo, SW_HIDE);
	}
	m_bVideoFlag = enable;
}

void CUcsCallFrame::SetCallTipInfo(const string &strTipsInfo)
{

	if ( NULL == m_pTxtCallInfo ) return ;

	m_strTipsInfo = strTipsInfo;
	m_pTxtCallInfo->SetText(strTipsInfo.c_str());
}

void CUcsCallFrame::SetCallerInfo(const CDuiString &strMyName, const CDuiString &strContactName,const CDuiString &strMyLogo, const CDuiString &strContactLogo)
{
	m_pTxtMyName->SetText(strMyName);
	m_pTxtContaceName->SetText(strContactName);

	if (!strMyLogo.IsEmpty())
	{
		m_pMyselfImage->SetBkImage(strMyLogo.GetData());
	}
	if (!strContactLogo.IsEmpty())
	{
		m_pContactImage->SetBkImage(strContactLogo.GetData());
	}

	if ( NULL != m_pSetTelVoice )
	{		
		m_pSetTelVoice->SetUserData("0");
		string strAtt = "normalimage=\"Call\\voice_nor.png\" hotimage=\"Call\\voice_hov.png\" pushedimage=\"Call\\voice_click.png\"";			
		m_pSetTelVoice->ApplyAttributeList(strAtt.c_str());		
	}
}


LRESULT CUcsCallFrame::OnKillWaitIncomingTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	StopUpdateCallStatus();

	return 0;
}

bool CUcsCallFrame::UpdateCallStatus()
{
	SetCallStatus(CALL_DIALLING);
	//m_pEditInput->SetText(L"");

	SetUIStatus(false);

	if ( NULL == m_pCallStatus ) return false;

	m_pCallStatus->SetVisible(true);

	return m_PaintManager.SetTimer(m_pCallStatus, TIMER_UPDATE_CALL_STATUS, 500);	
}

// 开始计时
bool CUcsCallFrame::UpdateCallTime()
{
	StopUpdateCallStatus();
	SetCallStatus(CALL_CALLING);
	//SetUIStatus(false);
	m_uCallTime = 0;
	return m_PaintManager.SetTimer(m_pTxtCallInfo, TIMER_UPDATE_CALL_TIME, 990);		
}

bool CUcsCallFrame::StopUpdateCallStatus()
{
	SetCallStatus(CALL_END);

	if ( NULL == m_pCallStatus ) return false;

	m_PaintManager.KillTimer(m_pCallStatus);	
	m_pCallStatus->SetVisible(false);
	m_pTxtCallInfo->SetText("");
	return true;
}


// 结束计时
bool CUcsCallFrame::StopUpdateCallTime()
{
	if ( NULL == m_pTxtCallInfo ) return false;
	m_PaintManager.KillTimer(m_pTxtCallInfo);	
	return true;
}

void CUcsCallFrame::SetCallStatus(int nStatus)
{
	EnterCriticalSection(&m_csStatu);
	m_nCallStatus = nStatus;
	LeaveCriticalSection(&m_csStatu);
}

int CUcsCallFrame::GetCallStatus()
{
	EnterCriticalSection(&m_csStatu);
	int iStatu = m_nCallStatus;
	LeaveCriticalSection(&m_csStatu);
	return iStatu;
}

void CUcsCallFrame::SetUIStatus(bool bNeedShowDBoard)
{
	bool bIsCalling = (GetCallStatus() != CALL_CALLING);
	//if ( NULL != m_pVerCallImage ) m_pVerCallImage->SetVisible(false);

	//if ( NULL != m_pVerTelLocPanel ) m_pVerTelLocPanel->SetVisible(!bNeedShowDBoard);	
	//if ( NULL != m_pVerDailBoard ) m_pVerDailBoard->SetVisible(bNeedShowDBoard);
}

void CUcsCallFrame::OnBtnClickHangup(void)
{
	if (CALL_END == GetCallStatus())
	{
		return;
	}
	StopUpdateCallStatus();
	StopUpdateCallTime();

	SetCallStatus(CALL_END);
	Ucs_hangUp("");
	ShowWindow(false);
	mainDlg::getInstance()->EnableCallBtn(true);	
}


bool CUcsCallFrame::OnSetVolume(bool bMicMute, unsigned nVolumeMic, unsigned nVolumeSound)
{
	//if ( NULL != m_pBnDialboard ) m_pBnDialboard->SetEnabled(true);

	Ucs_setMicMuteStatus(bMicMute);
	return true;
}

void CUcsCallFrame::OnCloseClick()
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
			/*	OnBtnClickHangup();
			CUcsMainFrame *pIns = CUcsMainFrame::Instance();
			if ( NULL == pIns ) return ;
			pIns->OnCallEnded(500);
			break;*/
		}
	}
	StopUpdateCallStatus();
	StopUpdateCallTime();
	ShowWindow(false);
	mainDlg::getInstance()->EnableCallBtn(true);	
}

LRESULT CUcsCallFrame::HandleCustomMessage( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
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

void CUcsCallFrame::InitWindow()
{
	m_pTxtCallInfo = m_PaintManager.FindControl(_T("TxtInfo"));
	m_pTxtMyName     = m_PaintManager.FindControl(_T("txtMyName"));
	m_pTxtContaceName     = m_PaintManager.FindControl(_T("txtContactName"));
	m_pCallStatus  = m_PaintManager.FindControl(_T("Call_Status"));
	m_pSetTelVoice = static_cast<CButtonUI*>(m_PaintManager.FindControl(_T("BnSetVoice")));
	m_pVerCallPanel   = static_cast<CVerticalLayoutUI*>(m_PaintManager.FindControl(_T("CallPanel")));
	m_pMyselfImage   = (m_PaintManager.FindControl(_T("MyselfImage")));
	m_pContactImage = m_PaintManager.FindControl(_T("ContactImage1"));

	m_localVideo = CreateWindow(_T("STATIC"), _T(""), WS_VISIBLE | WS_CHILD , 0, 0, 0, 0, m_PaintManager.GetPaintWindow(), NULL, NULL, NULL);
	m_remoteVideo = CreateWindow(_T("STATIC"), _T(""), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, m_PaintManager.GetPaintWindow(), NULL, NULL, NULL);
}

DuiLib::CDuiString CUcsCallFrame::GetSkinFile()
{
	return _T("AudioCallFrame.xml");
}

void CUcsCallFrame::HandleHangupEvent( int reason )
{
	//if (UCS_CALLER_HANGUP != reason /*&& CALL_END != GetCallStatus()*/)
	{
		CMsgBox* pMsgBox = new CMsgBox("", CUcPaasClient::GetUcsErrInfo(reason));
		pMsgBox->ShowModal(this->GetHWND());
		StopUpdateCallTime();
		StopUpdateCallStatus();
		SetCallStatus(CALL_END);	
		SetCallTipInfo("");	
		ShowWindow(false);
		mainDlg::getInstance()->EnableCallBtn(true);
	}	
}

void CUcsCallFrame::HandleAnswerEvent()
{
	StopUpdateCallStatus();
	UpdateCallTime();
	SetCallStatus(CALL_CALLING);
	//SetCallTipInfo("对方接听");
}

void CUcsCallFrame::HandleDialFailedEvent( int reason )
{
	SetCallStatus(CALL_NORMAL);
	CMsgBox* pMsgBox = new CMsgBox("",CUcPaasClient::GetUcsErrInfo(reason));
	pMsgBox->ShowModal(NULL);
}

void CUcsCallFrame::HandleAlertingEvent()
{
	if (CALL_END == GetCallStatus())
	{
		return;
	}
	SetCallStatus(CALL_DIALLING);
	SetCallTipInfo("等待对方接听");
}


void CUcsCallFrame::HandleClickEvent( TNotifyUI& msg )
{
	if ( msg.pSender->GetName() == _T("BnCancelCall") )
	{
		OnBtnClickHangup();			
	}	
	else if (msg.pSender->GetName() == _T("BnClose"))
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
			strAtt = "normalimage=\"Call\\no_voice_nor.png\" hotimage=\"Call\\no_voice_hov.png\" pushedimage=\"Call\\no_voice_click.png\"";					
		}	
		else
		{
			m_pSetTelVoice->SetUserData("0");
			strAtt = "normalimage=\"Call\\voice_nor.png\" hotimage=\"Call\\voice_hov.png\" pushedimage=\"Call\\voice_click.png\"";					
		}	

		m_pSetTelVoice->ApplyAttributeList(strAtt.c_str());
	}
}

void CUcsCallFrame::HandleTimerEvent( TNotifyUI& msg )
{
	if ( msg.pSender == m_pCallStatus ) 
	{
		if ( m_nStatusImgIdx > 4 ) m_nStatusImgIdx = 0;

		char szImage[64] = {0};
		sprintf(szImage, "Call\\calling_status%d.png", m_nStatusImgIdx++);
		m_pCallStatus->SetBkImage(szImage);
	}
	else if ( msg.pSender == m_pTxtCallInfo )
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

