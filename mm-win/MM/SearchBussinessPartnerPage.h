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

	//"��ѯ"��ť�¼�
	void OnQueryPartner();
	//"�Ӻ���"��ť�¼�
	void OnAddPartner();
	//���ͷ����ʾ������Ϣ
	void OnShowPartnerInfoPage();

	virtual void OnVCardReturned(tstring& strAccount, sUserVCard& oneInfo);
	virtual CDuiString GetSkinFile()			{	return _T("query_bussiness_partner_page.xml");		}
	//virtual void OnFinalMessage( HWND hWnd )	{	delete this;				}
	virtual void HandleClickEvent(TNotifyUI& msg);
	virtual void InitWindow();

	tstring FoundPhoneNo(){return m_strFindPhoneNo;}
	//�Լ���VCard��Ϣ
	virtual void OnMySelfCardInfo(sUserVCard& oneInfo);
	//İ���˵�VCard��Ϣ
	virtual void OnStrangerVCard(tstring& strAccount, sUserVCard& oneInfo);

protected:
	tstring m_strFindPhoneNo;
	HWND m_hMainWnd;
	sUserVCard* m_pCurUserInfo;
	CButtonUI* m_pBtnQuery;
	CSendAddRequestPage* m_pRequestpage;
	CViewFoundPartnerInfoPage* m_pViewPartnerInfoPage;
};