#pragma once
#include "mmwinimplbase.h"


struct busFileInfo
{
	tstring strFileName;                  //文件名 110
	tstring strBid;                       //业务号  90
	tstring strSendOrg;              //发送机构   130
	tstring  strSender;             //发送人 80
	tstring  strSendTime;                   //发送时间 160
	tstring   strRecvOrg;                  //接收机构 130
	tstring strRecvTime;                //接收时间 160
	                                  //操作100
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

