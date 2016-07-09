#pragma once
#include "MMWinImplBase.h"

#define CALL_ANSER	0
#define CALL_HANGUP 1
#define CLOSE_COMMING_DLG 2 

class CUcsIncomeingDlg : public MMWindowImplBase
{
public:
	CUcsIncomeingDlg(void);
	~CUcsIncomeingDlg(void);	

	void InitalCallInDlg(const tstring strNickname, const bool bVideo);
	UINT ShowModal();
	void CreateCallInDlg();
	
protected:  	
	virtual CDuiString GetSkinFile();
	virtual void InitWindow();
	virtual void HandleClickEvent(TNotifyUI& msg);
	//LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
	CLabelUI* m_pNickname;
	CLabelUI*  m_pTxtInfo;
	CControlUI* m_pImageCtrl;
	int nRetCode;
};
