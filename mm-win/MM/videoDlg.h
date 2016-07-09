#pragma once

#include "MMWinImplBase.h"
#include "UCSNetServiceCallBackInterface.h"
#include "../Common/UserInfoDef.h"
#include "../include/UsrManagerEvent_i.h"
#include "..\include\TrayTaskEvent_i.h"

class CUcsIncomeingDlg;

class CVideoDlg : public MMWindowImplBase, 
								public CUserManagerEventBase, 
								public CTrayTaskEventBase
{
public:	
	virtual CDuiString GetSkinFile();
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void InitWindow();
	bool LauchCall(const UCS_DIALTYPE eDialType);
	void ActiveVoiceDlg(bool bActive);
	bool IsActive();
	static CVideoDlg * Instance();
	static void Destroy();
	void ShowVideoWnd(bool bVisible);
	void InitalDialInfo(CDuiString strPhone, CDuiString strNickname,CDuiString strOrgName,CDuiString strAvatar);
	void EnableHangUp(bool bEnabled);	
	void SetVideoView(bool bVideoView);
	void SetCallerInfo();
	void SetCallTipInfo(const tstring strTipsInfo);
	void OnLauchNetCall(const CDuiString strAccount, int type);
	virtual void OnUserStateUpdated(tstring& strAccount, EUsrState eState, 
		tstring strRes, tstring strBusyInfo = "");
	void SetCallStatus(int nStatus);
		virtual void OnDbClickCallIncomeMessage(tstring strNewUser);
private:
	void HandleHangupEvent(int reason);
	void HandleAnswerEvent();
	void HandleDialFailedEvent(int reason);
	void HandleAlertingEvent();
	bool UpdateCallTime();
	bool StopUpdateCallTime();	
	int  GetCallStatus();
	void OnCloseClick();

protected:  		
	CVideoDlg(void);
	~CVideoDlg(void);

	void HandleClickEvent(TNotifyUI& msg);
	void HandleTimerEvent(TNotifyUI& msg);

	void OnBtnClickHangup(void);
	void SetMicMuteStatus(bool bIsMicMute);
	void SetLocalVideoStatus(bool bIsOpened);
	void SetWndPinStatus(bool bIsTop);
	void HandleExStatus(eExCallStatus eStatus);
	void HandleUcsConnFailedEvent(int reason);
	void HandleCallBackEvent(int reason);
	void HandleUcsConnSuccessEvent();
	void HandleIncomming(stUcsIncommingCall* lpUcsCallInfo);
private:
	CDuiString m_strPhone;
	CDuiString m_strNickname;
	CDuiString m_strOrgName;
	CDuiString m_strAvatar;
	HWND		m_localVideo;
	HWND		m_remoteVideo;
	static CVideoDlg* m_spInstant;
	bool  m_bIsActive;
	CLabelUI* m_pCaneraInfo;
	CLabelUI* m_pVideoCallStatus;
	CLabelUI* m_pCallTime;
	CLabelUI* m_pFrmCaption;
	CButtonUI* m_pSetTelVoice;
	CButtonUI* m_pBtnHangup;
	CButtonUI* m_pBtnEnterCurChatPage;
	CButtonUI* m_pBtnVolume;
	CButtonUI* m_pBtnCamera;
	CButtonUI* m_pPinTopWnd;
	CTabLayoutUI* m_pTabLayout;

	CLabelUI* m_pNickname;
	CLabelUI* m_pOrgNameLabel;
	CLabelUI* m_pVoiceCallStatus;
	CControlUI* m_pContactLogo;
	CRITICAL_SECTION	m_csStatu;
	tstring            m_strTipsInfo;
	UINT               m_uCallTime;
	int				   m_nCallStatus;
	bool m_bVideoView;
	bool m_bShowRemoteVideo;
	UCS_DIALTYPE m_ucsDialType;
	CUcsIncomeingDlg *m_pIncomeDlg;
public:
	enum eCallStatus{	
		CALL_NORMAL,		
		CALL_LAUCH_DIAL,
		CALL_DIALLING,
		CALL_CALLING,
		CALL_END,
	};
};
