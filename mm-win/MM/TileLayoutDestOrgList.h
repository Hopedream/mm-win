#pragma once

#include "StdAfx.h"
using namespace DuiLib;

class CTileLayoutDestOrgList :  public CListUI/*CTileLayoutUI*/
{
public:
	CTileLayoutDestOrgList(CPaintManagerUI* ppm=NULL);
	virtual ~CTileLayoutDestOrgList();

	bool Add(CControlUI *pControl);
	bool AddAt(CControlUI *pControl, int iIndex);
	bool Remove(CControlUI *pControl);
	bool RemoveAt(int iIndex);
	void RemoveAll();
	void DoEvent(TEventUI& event);


//	void AddOneItem(int nOrgID, tstring& strAvatar, tstring& strOrgName, tstring& strOnlineMems);
	void AddOneItem(tstring& nOrgID, tstring& strAvatar, tstring& strOrgName, tstring& strOnlineMems);
	//void DoEvent(TEventUI& event) ;
	
protected:
	DuiLib::CPaintManagerUI* m_pPaintManager;
	CDialogBuilder m_DlgBuilder;
};