#pragma once

#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"

using namespace DuiLib;
class CTileLayoutLocalParticipant :  public CTileLayoutUI/*, public INotifyUI*/
{
public:

	CTileLayoutLocalParticipant(CPaintManagerUI* ppm = NULL);
	virtual ~CTileLayoutLocalParticipant();


	void AddOneItem(sFriendListItem* pOneParticipant, vector<tstring>& vecProperty);

	void GetLocalParticipant(vector<tstring>& vecParticipant);

	void DeSelectItem(/*tstring& strAccount*/int nSrcItemID);

	void InitSelectState(tstring& strAccount);

	void DoEvent(TEventUI& event) ;
	//virtual void Notify(TNotifyUI& msg);

protected:
	DuiLib::CPaintManagerUI* m_pPaintManager;
};