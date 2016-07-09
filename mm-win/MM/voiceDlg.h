#pragma once

#include "MMWinImplBase.h"
#include "ucpaas/UCSClient.h"

class CVoiceDlg : public MMWindowImplBase
{
public:	
	virtual CDuiString GetSkinFile();
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void InitWindow();
	void LauchCall(UCS_DIALTYPE eDialType);
	void ActiveVoiceDlg(bool bActive);
	bool IsActive();
	static CVoiceDlg * Instance();


	void SetCallTipInfo(const string &strTipsInfo);

	void InitalDialInfo(CDuiString strPhone, CDuiString strNickname,CDuiString strOrgName,CDuiString strAvatar);

	bool UpdateCallTime();
	bool StopUpdateCallTime();

	void SetCallStatus(int nStatus);
	int  GetCallStatus();

	void OnCloseClick();
	void EnableHangUp(bool bEnabled);

private:
	void HandleHangupEvent(int reason);
	void HandleAnswerEvent();
	void HandleDialFailedEvent(int reason);
	void HandleAlertingEvent();



protected:  		
	//CVoiceDlg();
	CVoiceDlg(void);
	~CVoiceDlg(void);

	void HandleClickEvent(TNotifyUI& msg);
	void HandleTimerEvent(TNotifyUI& msg);

	void OnBtnClickHangup(void);
	bool OnSetVolume(bool bMicMute, unsigned nVolumeMic, unsigned nVolumeSound);			
	void SetCallerInfo(const CDuiString &strNickName, const CDuiString &strOrgName,const CDuiString &strLogo);
	

private:
	static CVoiceDlg* m_spInstant;
	CDuiString m_strPhone;
	CDuiString m_strNickname;
	CDuiString m_strOrgName;
	CDuiString m_strAvatar;
	bool  m_bIsActive;
	//CEditUI           *m_pEditInput;
	CLabelUI* m_pNickname;
	CLabelUI* m_pOrgNameLabel;
	CLabelUI* m_pTxtCallInfo;

	UCS_DIALTYPE m_ucsDialType;

	CControlUI* m_pContactLogo;
	CButtonUI* m_pSetTelVoice;
	CButtonUI* m_pBtnHangup;
	CRITICAL_SECTION	m_csStatu;
	string            m_strTipsInfo;
	UINT               m_uCallTime;
	int				   m_nCallStatus;

private:
	enum eCallStatus{	
		CALL_NORMAL,
		CALL_LAUCH_DIAL,
		CALL_DIALLING,
		CALL_CALLING,
		CALL_END,
	};
};
