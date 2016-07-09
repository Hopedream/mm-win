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
	else if (pSender->GetName() == _T ("btnTip")) {//�������˲��ҿ�����İ�ť(��������͵�),�����ص�ǰ��btn��������edit�Ľ���
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
	//�ҵ�main_dialog.xml����TabLayout �ؼ� SidebarTabContainer��
	//���ǲ��ǵ�������е�һ��ҳǩ������ǣ�����ʾ��SidebarTabContainer��������Ŀؼ�������
	//��SidebarTabContainer�ؼ�����������Tabҳǩ�Ĺ������ڶ�ȡmain_dialog.xml�����ļ���ʱ��
	//�ͽ����ˡ�TabLayout�Ǹ������������m_items�洢�ľ�����Щҳǩ��
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
	////���㶪ʧ�ˣ������ǲ����û�����˲��Ұ�ť������ǵģ�����ʾ���ҿ��ڵ�button�������ؼ�����ʾ��button�ϣ������������͵ġ������ġ�
	//if (pSender->GetName() == _T ("editSearch")) {
	//	CControlUI* pbtnTip = static_cast <CControlUI*> (m_pm.FindControl(_T ("btnTip")));
	//	CEditUI* peditSearch = static_cast <CEditUI*> (m_pm.FindControl(_T ("editSearch")));

	//	if (pbtnTip && peditSearch) {
	//		CDuiString strText = peditSearch->GetText();
	//		if (strText.IsEmpty()) {
	//			strText = "Ոݔ����җl��";
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


//���ڳ�ʼ����Ϻ󣬸���������Ŀؼ����г�ʼ����
void CMainDialog::OnInit()
{
	////��listctrl �ؼ��������У��ֱ����ͼ����ַ������ݡ�"No","Domain","Description"
	//CListUI* pList = static_cast <CListUI*> (m_pm.FindControl(_T ("lstNormal")));
	//ASSERT(pList);
	//if (pList) {
	//	pList->SetTextCallback(this);//�����ı�ȡ�صĻص�����==>LPCTSTR CMainDialog::GetItemText( CControlUI* pList, int iItem, int iSubItem )
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
	////��ʼ��tree�Ľڵ���Ϣ
	//AddTreeItem ();
}

LPCTSTR CMainDialog::GetItemText( CControlUI* pList, int iItem, int iSubItem )
{
	////UiList.cpp::void CListTextElementUI::DrawItemText(HDC hDC, const RECT& rcItem){ if( pCallback ) strText = pCallback->GetItemText(this, m_iIndex, i);}
	////������ƶ����ߵ��µ�list�ػ��ʱ�򣬾ͻ�ص��û��ĺ������ú�����ע������CMainDialog::OnInit(){pList->SetTextCallback(this);}��ɵġ�
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
	//			str.Format(_T ("˫����س��� %d ��"), pList->GetCurSel());
	//			MessageBox (GetHWND(), str, NULL, MB_OK);
	//		}
	//	} else if (msg.pSender && msg.pSender->GetParent () && msg.pSender->GetParent ()->GetParent () &&
	//		_tcsicmp (msg.pSender->GetParent ()->GetParent ()->GetClass(), "Tree") == 0)
	//	{
	//		CDuiTreeView* pTree1 = static_cast<CDuiTreeView*>(m_pm.FindControl(_T("Tree1")));
	//		if( pTree1 != NULL && pTree1->GetItemIndex(msg.pSender) != -1 )
	//		{
	//			//˫��ĳtree�Ľڵ㣬�����˵�ǰ�ڵ㣬��ʾ/���ظýڵ�������ӽڵ㡣
	//			if( _tcscmp(msg.pSender->GetClass(), _T("ListLabelElementUI")) == 0 ) {
	//				CDuiTreeView::Node* node = (CDuiTreeView::Node*)msg.pSender->GetTag();
	//				pTree1->SetChildVisible(node, !node->data()._child_visible);
	//			}
	//		}
	//	}
	//}

	//if( msg.sType == _T("itemclick") ) {//����û��Ƿ�����tree�е�һ���ڵ㡣
	//	if (msg.pSender && msg.pSender->GetParent () && msg.pSender->GetParent ()->GetParent () &&
	//		_tcsicmp (msg.pSender->GetParent ()->GetParent ()->GetClass(), "Tree") == 0)
	//	{
	//		//�û�������Զ����Tree�ؼ��ϵĽڵ��ˡ�
	//		CDuiTreeView* pTree1 = static_cast<CDuiTreeView*>(m_pm.FindControl(_T("Tree1")));
	//		if( pTree1 && pTree1->GetItemIndex(msg.pSender) != -1 )
	//		{
	//			if( _tcscmp(msg.pSender->GetClass(), _T("ListLabelElementUI")) == 0 ) {
	//				CDuiTreeView::Node* node = (CDuiTreeView::Node*)msg.pSender->GetTag();

	//				POINT pt = { 0 };
	//				::GetCursorPos(&pt);
	//				::ScreenToClient(m_pm.GetPaintWindow(), &pt);
	//				pt.x -= msg.pSender->GetX();
	//				SIZE sz = pTree1->GetExpanderSizeX(node);//�������Ҷ�ӽڵ㣬����ֵsz��=0.����sz=0��������Ͳ���ִ��
	//				if( pt.x >= sz.cx && pt.x < sz.cy )                     
	//					pTree1->SetChildVisible(node, !node->data()._child_visible);
	//			}
	//		}
	//	}
	//}
	////������listctrl����ͷ
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
	//						str.Format(_T ("������ͷ %d ��"), i);
	//						MessageBox (GetHWND(), str, NULL, MB_OK);
	//					}
	//				}
	//			}
	//		}
	//	}
	//}

	//�������Ϣ�������˺�ʣ�µ���Ϣ��������ȥ����
	CDuiDialog::OnNotify(msg);
}

//
//<?xml version="1.0" encoding="utf-8" standalone="yes" ?>
//<Window>
//	<HorizontalLayout name="HorizontalLayoutUI1" width="371" height="50" bkcolor="#FFFFFFFF" bkcolor2="#FFFFFFFF" bordercolor="#FFFFFFFF" focusbordercolor="#FF000000">
//		//�հ�ռλ�������ԷŸ�ͼ��ʲô�ġ�
//		<Control name="comboitemimage" float="true" pos="2,3,0,0" width="17" height="17" mouse="false" />
//		//��һ�����֣���վ����
//		<Text name="comboitemname" float="true" pos="24,3,0,0" width="371" height="17" mouse="false" textpadding="2,0,2,0" align="wrap" />
//		//�ڶ������֣���ʾ��ַ��
//		<Text name="comboitemlink" float="true" pos="24,25,0,0" width="371" height="28" mouse="false" textpadding="2,0,2,0" showhtml="true" align="wrap" />
//
//	</HorizontalLayout>
//</Window>
//���Զ����combo����һ��item.
void CMainDialog::AddComboboxItem()
{
	//CComboUI* pcbx2 = static_cast <CComboUI*> (m_pm.FindControl("combo2"));
	//assert (pcbx2);
	//if (pcbx2) {
	//	CDialogBuilder builder;
	//	CContainerUI* pItem = static_cast <CContainerUI*> (builder.Create(_T("comboitem.xml"), 0));
	//	//���õ�һ��Ԫ�أ�ͼ��
	//	pItem->GetItemAt(0)->SetBkImage(_T ("icon_home.png"));
	//	//���õڶ�������Ԫ�ص����ݣ����֡�
	//	if (pcbx2->GetCount() % 2 == 0) {
	//		pItem->GetItemAt(1)->SetText(_T ("�ٶ�һ�£�����֪����"));
	//		pItem->GetItemAt(2)->SetText(_T ("http://www.baidu.com"));
	//	}
	//	else {
	//		pItem->GetItemAt(1)->SetText(_T ("360���� - �ɾ�����ȫ�������ε���������"));
	//		pItem->GetItemAt(2)->SetText(_T ("http://www.so.com/"));
	//	}
	//	//�����Item����һ��tag������������
	//	pItem->SetTag(pcbx2->GetCount());

	//	//���Լ����¼�ί�и�mainDialog.
	//	pItem->OnEvent += MakeDelegate (this, &CMainDialog::OnClickItem);
	//	
	//	//����һ�У�
	//	pcbx2->Add(pItem);
	//}
}

//��ӦCombobox Item��������¼����Զ����combo2.
bool CMainDialog::OnClickItem (void* pParam)
{
	//TEventUI* pEvent = (TEventUI*)pParam;
	////����û�ѡ���˸�item���ͽ�item����Ϣ���뵽combo��Ӧ��edit�ؼ�����ȥ��
	//if (pEvent->Type == UIEVENT_BUTTONDOWN) {
	//	CComboUI* pcbx = static_cast <CComboUI*> (m_pm.FindControl(_T ("combo2")));
	//	CEditUI* pedit = static_cast <CEditUI*> (m_pm.FindControl(_T ("ComboEdit")));
	//	assert (pedit);
	//	CContainerUI*  pItem = static_cast <CContainerUI*> (pcbx->GetItemAt (pEvent->pSender->GetTag()));
	//	pedit->SetText(pItem->GetItemAt(2)->GetText());
	//}
	//else if (pEvent->Type == UIEVENT_MOUSEENTER) {//�������󣬸�����ʾ��
	//	pEvent->pSender->SetBkColor(0xFF00FF00);
	//}
	//else if (pEvent->Type == UIEVENT_MOUSELEAVE) {//����ȥ�󣬻ָ���ɫ��
	//	pEvent->pSender->SetBkColor(0xFFFFFFFF);
	//}
	return true;
}

void CMainDialog::AddTreeItem()//��Tree��ӽڵ㡣
{
	/*CDuiTreeView* pTree1 = static_cast<CDuiTreeView*>(m_pm.FindControl(_T("Tree1")));
	if (pTree1) {
		pTree1->SetDepth(4);
		pTree1->SetExpandImage(_T ("tree_expand.png"));
		CDuiTreeView::Node* pCategoryNode = NULL;
		CDuiTreeView::Node* pGameNode = NULL;
		CDuiTreeView::Node* pServerNode = NULL;
		CDuiTreeView::Node* pRoomNode = NULL;
		pCategoryNode = pTree1->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}�Ƽ���Ϸ"));
		for( int i = 0; i < 4; ++i )
		{
			pGameNode = pTree1->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}���˶�����"), pCategoryNode);
			for( int i = 0; i < 3; ++i )
			{
				pServerNode = pTree1->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}���Է�����"), pGameNode);
				for( int i = 0; i < 3; ++i )
				{
					pRoomNode = pTree1->AddNode(_T("{x 4}{i gameicons.png 18 10}{x 4}���Է���"), pServerNode);
				}
			}
		}
		pCategoryNode = pTree1->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}����������Ϸ"));
		for( int i = 0; i < 2; ++i )
		{
			pTree1->AddNode(_T("��ȱһ"), pCategoryNode);
		}
		pCategoryNode = pTree1->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}������Ϸ"));
		for( int i = 0; i < 8; ++i )
		{
			pTree1->AddNode(_T("˫��"), pCategoryNode);
		}
		pCategoryNode = pTree1->AddNode(_T("{x 4}{i gameicons.png 18 3}{x 4}������Ϸ"));
		for( int i = 0; i < 8; ++i )
		{
			pTree1->AddNode(_T("������"), pCategoryNode);
		}
	}*/
}
