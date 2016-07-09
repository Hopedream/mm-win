#pragma once

#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"
#include "MMWinImplBase.h"
#include "../include/UsrManagerEvent_i.h"



class CSendAddRequestPage;
class CViewFoundPartnerInfoPage;
class CSearchBussinessPartnerPage : public MMWindowImplBase, public CUserManagerEventBase
{
public:
	CSearchBussinessPartnerPage();
	virtual ~CSearchBussinessPartnerPage();

	//void CreateWnd();
	bool Initialize();
	void SetMainWndHWND(HWND hWndMaind);
	void SetSearchResult(sUserVCard& oneUesr);
	void ClearSearchResult();

	//"查询"按钮事件
	void OnQueryPartner();
	//"加好友"按钮事件
	void OnAddPartner();
	//点击头像显示好友信息
	void OnShowPartnerInfoPage();

	virtual void OnVCardReturned(tstring& strAccount, sUserVCard& oneInfo);
	virtual CDuiString GetSkinFile()			{	return _T("query_bussiness_partner_page.xml");		}
	//virtual void OnFinalMessage( HWND hWnd )	{	delete this;				}
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void InitWindow();

	tstring FoundPhoneNo(){return m_strFindPhoneNo;}
	//自己的VCard信息
	virtual void OnMySelfCardInfo(sUserVCard& oneInfo);
	//陌生人的VCard信息
	virtual void OnStrangerVCard(tstring& strAccount, sUserVCard& oneInfo);

protected:
	tstring m_strFindPhoneNo;
	HWND m_hMainWnd;
	sUserVCard* m_pCurUserInfo;
	CButtonUI* m_pBtnQuery;
	CSendAddRequestPage* m_pRequestpage;
	CViewFoundPartnerInfoPage* m_pViewPartnerInfoPage;
};