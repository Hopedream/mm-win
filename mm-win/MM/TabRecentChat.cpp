#include "stdafx.h"
#include "TabRecentChat.h"
#include "CDialogBuilderCallbackEx.h"

CTabRecentChat::CTabRecentChat(CPaintManagerUI* ppm)
{
	CDialogBuilder builder;
	CContainerUI* pTabRecentChat = static_cast<CContainerUI*>(builder.Create(_T("tab_recent_new.xml"), 0, NULL, ppm));
	if (NULL != pTabRecentChat)
	{
		this->Add(pTabRecentChat);
	}
	else
	{
		this->RemoveAll();
		return;
	}
}