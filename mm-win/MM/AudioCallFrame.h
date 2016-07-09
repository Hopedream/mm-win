#pragma once

#include "MMWinImplBase.h"



class CUcsCallFrame : public MMWindowImplBase
{
public:
	virtual CDuiString GetSkinFile();
	virtual LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual void InitWindow();

	static CUcsCallFrame * Instance();


	void SetCallTipInfo(const string &strTipsInfo);
	void ShowVideo(bool enable);

	int SetVideoWindow();

	void SetCallerInfo(const CDuiString &strMyName, const CDuiString &strContactName,const CDuiString &strMyLogo, const CDuiString &strContactLogo);
	bool UpdateCallStatus();
	bool StopUpdateCallStatus();

	bool UpdateCallTime();
	bool StopUpdateCallTime();

	void SetCallStatus(int nStatus);
	int  GetCallStatus();

	void OnCloseClick();
	void SetUIStatus(bool bNeedShowDBoard);
private:
	void HandleHangupEvent(int reason);
	void HandleAnswerEvent();
	void HandleDialFailedEvent(int reason);
	void HandleAlertingEvent();
private: 
	CUcsCallFrame(void);
	~CUcsCallFrame(void);

protected:  	
	//UINT    GetClassStyle() const      { return CS_DBLCLKS; }	
	//void    OnFinalMessage(HWND hWnd)  { delete this; }
	//LPCTSTR GetWindowClassName() const { return _T("UcsCallFrameUI"); }

	//void Notify(TNotifyUI& msg);	
	void HandleClickEvent(TNotifyUI& msg);
	void HandleTimerEvent(TNotifyUI& msg);

	//LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	////LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);	
	////LRESULT /*OnCreate*/(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);	
	//LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnKillWaitIncomingTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void OnBtnClickHangup(void);
	//bool OnSendDTMF(string wstrDTMF);
	bool OnSetVolume(bool bMicMute, unsigned nVolumeMic, unsigned nVolumeSound);	
private:
	static CUcsCallFrame *m_spInstant;

	//CEditUI           *m_pEditInput;
	CControlUI        *m_pTxtMyName;
	CControlUI        *m_pTxtContaceName;
	CControlUI		  *m_pTxtCallInfo;
	CControlUI		  *m_pBnDialboard;
	CControlUI		  *m_pCallStatus;

	HWND		m_localVideo;
	HWND		m_remoteVideo;
	CControlUI		  *m_pMyselfImage;
	CControlUI		  *m_pContactImage;
	CButtonUI         *m_pSetTelVoice;


	CVerticalLayoutUI *m_pVerCallPanel;
	//CVerticalLayoutUI *m_pVerTelLocPanel;
	
	//CVerticalLayoutUI *m_pVerCallImage;
	//CVerticalLayoutUI *m_pVerDailBoard;

	CRITICAL_SECTION	m_csStatu;
	string            m_strTipsInfo;
	UINT               m_uCallTime;
	int				   m_nCallStatus;
	int                m_nEditCurPos;
	bool				m_bVideoFlag;
private:
	enum eCallStatus{	
		CALL_NORMAL,
		CALL_DIALLING,
		CALL_CALLING,
		CALL_END,
	};
	int m_nStatusImgIdx;
};
