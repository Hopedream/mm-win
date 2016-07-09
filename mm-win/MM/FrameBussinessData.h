#pragma once
#include "mmwinimplbase.h"


struct busFileInfo
{
	tstring strFileName;                  //�ļ��� 110
	tstring strBid;                       //ҵ���  90
	tstring strSendOrg;              //���ͻ���   130
	tstring  strSender;             //������ 80
	tstring  strSendTime;                   //����ʱ�� 160
	tstring   strRecvOrg;                  //���ջ��� 130
	tstring strRecvTime;                //����ʱ�� 160
	                                  //����100
};


class CFrameBussinessData :
	public MMWindowImplBase
{
public:
	CFrameBussinessData(void);
	~CFrameBussinessData(void);
	static CFrameBussinessData* Instance();

public:
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void HandleSelectedChangedEvent(TNotifyUI& msg);
	virtual void HandleItemSelectEvent(TNotifyUI& msg);
	virtual CDuiString GetSkinFile();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);
	virtual void InitWindow();
	bool AddItem(busFileInfo memInfo);
private:
	void onButtonFind();


private:
	static CFrameBussinessData* m_pInstance;
	CListUI* m_plstBussData;

	CEditUI *m_pedit_fileName;
	CEditUI *m_pedit_Bid;
	CEditUI *m_pedit_SendOrg;
	CEditUI *m_pedit_RecvOrg;
	CEditUI *m_pedit_Sender;
	CDateTimeUI *m_date_Send_begin;
	CDateTimeUI *m_date_Send_end;
	CDateTimeUI *m_date_Recv_begin;
	CDateTimeUI *m_date_Recv_end;
};

