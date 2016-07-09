#include "SysMsgPage.h"
#include "Utility.h"
#include "MsgBox.h"
#include <sstream>
#include "CDialogBuilderCallbackEx.h"
#include "..\Include\Global_Def.h"

using namespace std;

CSysMsgPage::CSysMsgPage(DuiLib::CPaintManagerUI* ppm)
{
	m_PaintManager = ppm;
	
	CDialogBuilder DlgBuilder1;
	CDialogBuilderCallbackEx cb1(m_PaintManager);
	CTabLayoutUI*pLayout = static_cast<CTabLayoutUI*>(m_PaintManager->FindControl("RecentChatTabLayout"));
	if (NULL == pLayout)
	{
		return;
	}

	//1.加载系统信息的page页面
	CHorizontalLayoutUI* pSysMsgPage = NULL;
	if (!DlgBuilder1.GetMarkup()->IsValid())
	{
		pSysMsgPage = static_cast<CHorizontalLayoutUI*>(DlgBuilder1.Create(_T("tab_sys_msg_page.xml"),
			(UINT)0, &cb1, m_PaintManager));
	}
	else
	{
		pSysMsgPage = static_cast<CHorizontalLayoutUI*>(DlgBuilder1.Create((UINT)0, m_PaintManager));
	}

	//作为第2个页签
	if (NULL != pSysMsgPage)
	{
		pLayout->AddAt(pSysMsgPage, CONST_RECENT_TAB_SYS_MSG_PAGE/*1*/);
	}
}

CSysMsgPage::~CSysMsgPage()
{

}

void CSysMsgPage::DoEvent(TEventUI& event)
{
	if (event.Type == UIEVENT_BUTTONDOWN )
	{
		tstring strSender = event.pSender->GetName();

		if (event.pSender->GetName() == "btn_netcall_free")
		{
		}
		else if (event.pSender->GetName() == "btn_callbyphone")
		{
		}
	}
	__super::DoEvent(event);
}