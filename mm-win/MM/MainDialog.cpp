#include "StdAfx.h"
#include "MainDialog.h"
// #include "MyList.h"
// #include "TreeView.h"
#include <exdisp.h>
#include <comdef.h>


CMainDialog::CMainDialog(void)
{
}


CMainDialog::~CMainDialog(void)
{
}

void CMainDialog::OnClick( CControlUI* pSender)
{
	if (pSender->GetName() == TEXT ("btnExit")) {
		PostQuitMessage(0);
		return;
	}
	else if (pSender->GetName() == _T ("btnTip")) {//如果点击了查找框里面的按钮(鼠标是手型的),就隐藏当前的btn，并设置edit的焦点
		CControlUI* pbtnTip = static_cast <CControlUI*> (m_pm.FindControl(_T ("btnTip")));
		if (pbtnTip) {
			pbtnTip->SetVisible(false);
		}

		CEditUI* peditSearch = static_cast <CEditUI*> (m_pm.FindControl(_T ("editSearch")));
		if (peditSearch != NULL) {
			peditSearch->SetFocus();
		}
	}
}

void CMainDialog::OnSelChanged( CControlUI* pSender )
{
	//找到main_dialog.xml里面TabLayout 控件 SidebarTabContainer。
	//看是不是点击了其中的一个页签，如果是，就显示该SidebarTabContainer容器里面的控件索引，
	//该SidebarTabContainer控件和他关联的Tab页签的关联是在读取main_dialog.xml配置文件的时候，
	//就建立了。TabLayout是个容器，里面的m_items存储的就是这些页签。
	CTabLayoutUI* pSidebarTabs = static_cast <CTabLayoutUI*>(m_pm.FindControl(_T ("SidebarTabContainer")));
	if (pSidebarTabs == NULL) {
		return;
	}

	CDuiString strSelName = pSender->GetName();
	if (strSelName == _T ("TabRecentChatList")) {
		pSidebarTabs->SelectItem(0);
	}
	else if (strSelName == _T ("TabProject")) {
		pSidebarTabs->SelectItem(1);
	}
	else if (strSelName == _T ("TabBusinessPartner")) {
		pSidebarTabs->SelectItem(2);
	}
	else if (strSelName == _T ("TabMoney")) {
		pSidebarTabs->SelectItem(3);
	}
	else if (strSelName == _T ("TabFinance")) {
		pSidebarTabs->SelectItem(4);
	}
	else if (strSelName == _T ("TabSysSetting")) {


		//pSidebarTabs->SelectItem(4);
	}
	else if (strSelName == _T ("TabCollection")) {
		pSidebarTabs->SelectItem(5);
	}
}

void CMainDialog::OnKillFocus( CControlUI* pSender )
{
	////焦点丢失了，看下是不是用户点击了查找按钮。如果是的，就显示查找框内的button，并将关键词显示在button上，这样就是手型的。高亮的。
	//if (pSender->GetName() == _T ("editSearch")) {
	//	CControlUI* pbtnTip = static_cast <CControlUI*> (m_pm.FindControl(_T ("btnTip")));
	//	CEditUI* peditSearch = static_cast <CEditUI*> (m_pm.FindControl(_T ("editSearch")));

	//	if (pbtnTip && peditSearch) {
	//		CDuiString strText = peditSearch->GetText();
	//		if (strText.IsEmpty()) {
	//			strText = "請輸入查找條件";
	//			pbtnTip->SetAttribute("textcolor", "#00A0A0A4");
	//		} else {
	//			pbtnTip->SetAttribute("textcolor", "#FF000000");
	//		}
	//		
	//		pbtnTip->SetText(strText);
	//		pbtnTip->SetVisible(true);
	//	}
	//}
}


//窗口初始化完毕后，给窗口里面的控件进行初始化。
void CMainDialog::OnInit()
{
	////给listctrl 控件，三个列，分别放置图标和字符串内容。"No","Domain","Description"
	//CListUI* pList = static_cast <CListUI*> (m_pm.FindControl(_T ("lstNormal")));
	//ASSERT(pList);
	//if (pList) {
	//	pList->SetTextCallback(this);//设置文本取回的回调函数==>LPCTSTR CMainDialog::GetItemText( CControlUI* pList, int iItem, int iSubItem )
	//	for (int i = 0; i < 100; ++i) {
	//		CListTextElementUI* pItem = new CListTextElementUI;
	//		m_arstrListString0 [i].Format(_T ("{i list_png.png}{/i}{x 4}%d"), i);
	//		m_arstrListString1 [i].Format(_T ("%d - 1"), i);
	//		m_arstrListString2 [i].Format(_T ("%d - 2"), i);
	//		pList->Add(pItem);
	//	}
	//}

	////
	//AddComboboxItem ();
	//
	////初始化tree的节点信息
	//AddTreeItem ();
}

LPCTSTR CMainDialog::GetItemText( CControlUI* pList, int iItem, int iSubItem )
{
	////UiList.cpp::void CListTextElementUI::DrawItemText(HDC hDC, const RECT& rcItem){ if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, i);}
	////当鼠标移动或者导致的list重绘的时候，就会回调用户的函数。该函数的注册是在CMainDialog::OnInit(){pList->SetTextCallback(this);}完成的。
	//if (iSubItem == 0) {
	//	return m_arstrListString0 [iItem];
	//}
	//else if (iSubItem == 1) {
	//	return m_arstrListString1 [iItem];
	//}
	//else if (iSubItem == 2) {
	//	return m_arstrListString2 [iItem];
	//}

	return _T (_T(""));
}

void CMainDialog::OnNotify( TNotifyUI& msg )
{
	//if (msg.sType == _T ("itemactivate")) {
	//	if (_tcsicmp (msg.pSender->GetClass(), "ListUI") == 0) {
	//		CListUI* pList = static_cast <CListUI*> (m_pm.FindControl(_T ("lstNormal")));
	//		ASSERT(pList);
	//		if (pList) {
	//			CDuiString str;
	//			str.Format(_T ("双击或回车了 %d 项"), pList->GetCurSel());
	//			MessageBox (GetHWND(), str, NULL, MB_OK);
	//		}
	//	} else if (msg.pSender && msg.pSender->GetParent () && msg.pSender->GetParent ()->GetParent () &&
	//		_tcsicmp (msg.pSender->GetParent ()->GetParent ()->GetClass(), "Tree") == 0)
	//	{
	//		CDuiTreeView* pTree1 = static_cast<CDuiTreeView*>(m_pm.FindControl(_T("Tree1")));
	//		if( pTree1 != NULL && pTree1->GetItemIndex(msg.pSender) != -1 )
	//		{
	//			//双击某tree的节点，激活了当前节点，显示/隐藏该节点的所有子节点。
	//			if( _tcscmp(msg.pSender->GetClass(), _T("ListLabelElementUI")) == 0 ) {
	//				CDuiTreeView::Node* node = (CDuiTreeView::Node*)msg.pSender->GetTag();
	//				pTree1->SetChildVisible(node, !node->data()._child_visible);
	//			}
	//		}
	//	}
	//}

	//if( msg.sType == _T("itemclick") ) {//检查用户是否点击了tree中的一个节点。
	//	if (msg.pSender && msg.pSender->GetParent () && msg.pSender->GetParent ()->GetParent () &&
	//		_tcsicmp (msg.pSender->GetParent ()->GetParent ()->GetClass(), "Tree") == 0)
	//	{
	//		//用户点击了自定义的Tree控件上的节点了。
	//		CDuiTreeView* pTree1 = static_cast<CDuiTreeView*>(m_pm.FindControl(_T("Tree1")));
	//		if( pTree1 && pTree1->GetItemIndex(msg.pSender) != -1 )
	//		{
	//			if( _tcscmp(msg.pSender->GetClass(), _T("ListLabelElementUI")) == 0 ) {
	//				CDuiTreeView::Node* node = (CDuiTreeView::Node*)msg.pSender->GetTag();

	//				POINT pt = { 0 };
	//				::GetCursorPos(&pt);
	//				::ScreenToClient(m_pm.GetPaintWindow(), &pt);
	//				pt.x -= msg.pSender->GetX();
	//				SIZE sz = pTree1->GetExpanderSizeX(node);//如果不是叶子节点，返回值sz！=0.否则，sz=0下面的语句就不会执行
	//				if( pt.x >= sz.cx && pt.x < sz.cy )                     
	//					pTree1->SetChildVisible(node, !node->data()._child_visible);
	//			}
	//		}
	//	}
	//}
	////单击了listctrl的列头
	//if (stricmp (msg.pSender->GetClass(), _T ("ListHeaderItemUI")) == 0 && msg.sType == _T ("headerclick")) {
	//	CListUI* pList = static_cast <CListUI*> (m_pm.FindControl(_T ("lstNormal")));
	//	ASSERT(pList);
	//	if (pList) {
	//		CListHeaderUI* pHeader = pList->GetHeader();
	//		ASSERT(pHeader);
	//		if (pHeader) {
	//			for (int i = 0; i < pHeader->GetCount(); ++i) {
	//				CControlUI* pItem = pHeader->GetItemAt (i);
	//				ASSERT(pItem);
	//				if (pItem) {
	//					if (PtInRect (&pItem->GetPos(), msg.ptMouse)) {
	//						CDuiString str;
	//						str.Format(_T ("单击了头 %d 项"), i);
	//						MessageBox (GetHWND(), str, NULL, MB_OK);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//上面的消息处理完了后，剩下的消息交给子类去处理。
	CDuiDialog::OnNotify(msg);
}

//
//<?xml version="1.0" encoding="utf-8" standalone="yes" ?>
//<Window>
//	<HorizontalLayout name="HorizontalLayoutUI1" width="371" height="50" bkcolor="#FFFFFFFF" bkcolor2="#FFFFFFFF" bordercolor="#FFFFFFFF" focusbordercolor="#FF000000">
//		//空白占位符，可以放个图标什么的。
//		<Control name="comboitemimage" float="true" pos="2,3,0,0" width="17" height="17" mouse="false" />
//		//第一行文字：网站名称
//		<Text name="comboitemname" float="true" pos="24,3,0,0" width="371" height="17" mouse="false" textpadding="2,0,2,0" align="wrap" />
//		//第二行文字：显示网址。
//		<Text name="comboitemlink" float="true" pos="24,25,0,0" width="371" height="28" mouse="false" textpadding="2,0,2,0" showhtml="true" align="wrap" />
//
//	</HorizontalLayout>
//</Window>
//给自定义的combo新增一个item.
void CMainDialog::AddComboboxItem()
{
	//CComboUI* pcbx2 = static_cast <CComboUI*> (m_pm.FindControl("combo2"));
	//assert (pcbx2);
	//if (pcbx2) {
	//	CDialogBuilder builder;
	//	CContainerUI* pItem = static_cast <CContainerUI*> (builder.Create(_T("comboitem.xml"), 0));
	//	//设置第一个元素：图标
	//	pItem->GetItemAt(0)->SetBkImage(_T ("icon_home.png"));
	//	//设置第二，三个元素的内容：文字。
	//	if (pcbx2->GetCount() % 2 == 0) {
	//		pItem->GetItemAt(1)->SetText(_T ("百度一下，您就知道了"));
	//		pItem->GetItemAt(2)->SetText(_T ("http://www.baidu.com"));
	//	}
	//	else {
	//		pItem->GetItemAt(1)->SetText(_T ("360搜索 - 干净、安全、可信任的搜索引擎"));
	//		pItem->GetItemAt(2)->SetText(_T ("http://www.so.com/"));
	//	}
	//	//给这个Item设置一个tag，方便索引。
	//	pItem->SetTag(pcbx2->GetCount());

	//	//将自己的事件委托给mainDialog.
	//	pItem->OnEvent += MakeDelegate (this, &CMainDialog::OnClickItem);
	//	
	//	//新增一行！
	//	pcbx2->Add(pItem);
	//}
}

//响应Combobox Item被点击的事件。自定义的combo2.
bool CMainDialog::OnClickItem (void* pParam)
{
	//TEventUI* pEvent = (TEventUI*)pParam;
	////如果用户选择了改item。就将item的信息放入到combo对应的edit控件里面去。
	//if (pEvent->Type == UIEVENT_BUTTONDOWN) {
	//	CComboUI* pcbx = static_cast <CComboUI*> (m_pm.FindControl(_T ("combo2")));
	//	CEditUI* pedit = static_cast <CEditUI*> (m_pm.FindControl(_T ("ComboEdit")));
	//	assert (pedit);
	//	CContainerUI*  pItem = static_cast <CContainerUI*> (pcbx->GetItemAt (pEvent->pSender->GetTag()));
	//	pedit->SetText(pItem->GetItemAt(2)->GetText());
	//}
	//else if (pEvent->Type == UIEVENT_MOUSEENTER) {//鼠标进来后，高亮显示。
	//	pEvent->pSender->SetBkColor(0xFF00FF00);
	//}
	//else if (pEvent->Type == UIEVENT_MOUSELEAVE) {//鼠标出去后，恢复底色。
	//	pEvent->pSender->SetBkColor(0xFFFFFFFF);
	//}
	return true;
}

void CMainDialog::AddTreeItem()//给Tree添加节点。
{
	/*CDuiTreeView* pTree1 = static_cast<CDuiTreeView*>(m_pm.FindControl(_T("Tree1")));
	if (pTree1) {
		pTree1->SetDepth(4);
		pTree1->SetExpandImage(_T ("tree_expand.png"));
		CDuiTreeView::Node* pCategoryNode = NULL;
		CDuiTreeView::Node* pGameNode = NULL;
		CDuiTreeView::Node* pServerNode = NULL;
		CDuiTreeView::Node* pRoomNode = NULL;
		pCategoryNode = pTree1->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}推荐游戏"));
		for( int i = 0; i < 4; ++i )
		{
			pGameNode = pTree1->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}四人斗地主"), pCategoryNode);
			for( int i = 0; i < 3; ++i )
			{
				pServerNode = pTree1->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}测试服务器"), pGameNode);
				for( int i = 0; i < 3; ++i )
				{
					pRoomNode = pTree1->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}测试房间"), pServerNode);
				}
			}
		}
		pCategoryNode = pTree1->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}最近玩过的游戏"));
		for( int i = 0; i < 2; ++i )
		{
			pTree1->AddNode(_T("三缺一"), pCategoryNode);
		}
		pCategoryNode = pTree1->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}棋牌游戏"));
		for( int i = 0; i < 8; ++i )
		{
			pTree1->AddNode(_T("双扣"), pCategoryNode);
		}
		pCategoryNode = pTree1->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}休闲游戏"));
		for( int i = 0; i < 8; ++i )
		{
			pTree1->AddNode(_T("飞行棋"), pCategoryNode);
		}
	}*/
}
