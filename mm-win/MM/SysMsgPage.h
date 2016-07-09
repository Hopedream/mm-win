#pragma once

#include "StdAfx.h"
#include <string>

using namespace DuiLib;
using namespace std;

class CSysMsgPage : public CHorizontalLayoutUI
{
public:
	CSysMsgPage(CPaintManagerUI* ppm = NULL);
	virtual ~CSysMsgPage();

	//bool Initialize(tstring strAccount, sUserVCard& pUserInfo);
	//void UpdataMemo(tstring strNewMemo);
	void DoEvent(TEventUI& event);
public:
	//std::tstring m_strCurAccount;

protected:
	DuiLib::CPaintManagerUI* m_PaintManager;
};