#include "CDialogBuilderCallbackEx.h"
#include "UIContacts.h"
//#include "TabRecentChat.h"
#include "UIDataList.h"
#include "UIChatList.h"
#include "UIFriendList.h"
#include "SysMsgListCtrl.h"
#include "ListResPackets.h"
#include "TileLayoutLocalParticipant.h"
#include "TileLayoutResPacket.h"
#include "WkeWebkit.h"
#include "UIBussinessDataList.h"

CDialogBuilderCallbackEx::CDialogBuilderCallbackEx(CPaintManagerUI* ppm)
{
	m_pm = ppm;
}


CDialogBuilderCallbackEx::~CDialogBuilderCallbackEx(void)
{
}
//�������Զ���Ŀؼ�������������ط���
CControlUI* CDialogBuilderCallbackEx::CreateControl( LPCTSTR pstrClass )
{
	//������Ϊ����Զ�ȡmain_dialog.xml����ȡTablayout�ڵ㣬����̬����Tabҳǩʱ��Ĵ���
	////"�����ϵ��"ҳǩ
	//if (_tcsicmp(pstrClass, _T("TabRecentChatList")) == 0)
	//{
	//	return new CTabRecentChat;
	//}
	////"��Ŀ"ҳǩ
	//else if (_tcsicmp(pstrClass, _T("TabProject")) == 0)
	//{
	//}
	////"��ҵ���"ҳǩ
	//else if (_tcsicmp(pstrClass, _T("TabBusinessPartner")) == 0)
	//{

	//}
	////"Ǯ��"ҳǩ
	//else if (_tcsicmp(pstrClass, _T("TabMoney")) == 0)
	//{

	//}
	////"�ղ�"ҳǩ
	//else if(_tcsicmp(pstrClass, _T("TabCollection")) ==0)
	//{

	//}
	////"ϵͳ����"ҳǩ
	//else if (_tcsicmp(pstrClass, _T("TabSysSetting")) ==0)
	//{

	//}
	if (_tcsicmp(pstrClass, _T("ContactList")) == 0)
	{
		return new CContactsUI(m_pm);
	}
	else if (_tcsicmp(pstrClass, _T("DataList")) == 0)
	{
		return new CDataListUI(m_pm);
	}
	else if (_tcsicmp(pstrClass, _T("ChatList")) == 0)
	{
		return new CChatListUI(m_pm);
	}	
	else if (_tcsicmp(pstrClass, _T("AllFriendsList")) == 0)
	{
		return new CUIFriendList(m_pm);
	}
	else if (_tcsicmp(pstrClass, _T("BussinessDataList")) == 0)
	{
		return new CUIBussinessDataList(*m_pm);
	}
	else if (_tcsicmp(pstrClass, _T("GroupFileList")) == 0)
	{
		return new CListResPacket(*m_pm);
	}
	else if (_tcsicmp(pstrClass, _T("InviteFriendList")) == 0)
	{
		return new CListSysMsg(m_pm);
	}
	else if (_tcsicmp(pstrClass, _T("ListLocalParticipant")) == 0)
	{
		return new CTileLayoutLocalParticipant();
	}
	else if (_tcsicmp(pstrClass, _T("ListResourcePacket")) == 0)
	{
		return new CTileLayoutResPacket(m_pm);
	}
	else if (_tcsicmp(pstrClass, _T("wkeWebkit")) == 0)
	{
		return new CWkeWebkitUI();
	}
	
	return NULL;
}
