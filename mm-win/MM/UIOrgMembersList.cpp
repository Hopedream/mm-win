#include "UIOrgMembersList.h"


const int kFriendListItemNormalHeight	= 50;
const int kGroupItemHeight	= 30;

const TCHAR* const kFriendAvatar		= _T("friendAvatar");		//好友头像
const TCHAR* const kLogoContainerControlName			= _T("logo_container");			
const TCHAR* const kGroupCtrlName	= _T("groupName");	
const TCHAR* const kNickNameCtrlName	= _T("friendName");	

CUIOrgMembersList::CUIOrgMembersList( CPaintManagerUI* paint_manager )
	:CFriendListBaseUI(*paint_manager)
{

}



Node* CUIOrgMembersList::AddNode(const FriendListItemInfo& item, Node* parent)
{
	if (!parent)
		parent = root_node_;

	TCHAR szBuf[MAX_PATH] = {0};

	CListContainerElementUI* pListElement = NULL;
	if( !m_dlgBuilder.GetMarkup()->IsValid() ) 
	{
		pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create(_T("create_project_opposite_employee.xml"), 
						(UINT)0, NULL, &paint_manager_));
	}
	else 
	{
		pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create((UINT)0, &paint_manager_));
	}

	if (pListElement == NULL)
		return NULL;

	Node* node = new Node;

	int nParentLevel = parent->data().level_;
	bool bEmpty = item.empty;
	bool bFolder = item.folder;
	
	node->data().level_ = parent->data().level_ + 1;
	if (item.folder)
		node->data().has_child_ = !item.empty;
	else
		node->data().has_child_ = false;

	node->data().folder_ = item.folder;

	node->data().child_visible_ = (node->data().level_ == 0);
	node->data().child_visible_ = false;

	node->data().text_ = item.strOrgName;
	node->data().value = item.itemId;
	node->data().list_elment_ = pListElement;

	if (!parent->data().child_visible_)
		pListElement->SetVisible(false);

	if (parent != root_node_ && !parent->data().list_elment_->IsVisible())
		pListElement->SetVisible(false);

	CControlUI* log_button = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListElement, "control_oppo_unselect_avatar"));
	if (log_button != NULL)
	{
		if (!item.folder && !item.logo.empty())
		{
			log_button->SetBkImage(item.logo.c_str());

			CContainerUI* logo_container_org = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pListElement, "container_org"));
			if (logo_container_org != NULL)
				logo_container_org->SetVisible(false);

			CContainerUI* logo_container_mem = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pListElement, "container_friend"));
			if (logo_container_mem != NULL)
				logo_container_mem->SetVisible(true);
		}
		else
		{
			CContainerUI* logo_container_org = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pListElement, "container_org"));
			if (logo_container_org != NULL)
				logo_container_org->SetVisible(true);


			CContainerUI* logo_container_mem = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pListElement, "container_friend"));
			if (logo_container_mem != NULL)
				logo_container_mem->SetVisible(false);
		}
	}

	tstring html_text;
	if (node->data().has_child_)
	{
		if (node->data().child_visible_)
			html_text += level_expand_image_;
		else
			html_text += level_collapse_image_;
		_stprintf_s(szBuf, MAX_PATH - 1, _T("<x %d>"), level_text_start_pos_);
		html_text += szBuf;
	}

	if (item.folder)
	{
		html_text += node->data().text_;
	}
	else
	{
		_stprintf_s(szBuf, MAX_PATH - 1, /*_T("<f 2>%s</f>")*/_T("%s"), item.nick_name.c_str());
		html_text += szBuf;
	}

	CLabelUI* nick_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, "label_oppo_unselect_name"));

	CLabelUI* group_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, "groupName"));


	if (item.folder)
	{
		group_name->SetVisible(true);
		nick_name->SetVisible(false);
		group_name->SetText(html_text.c_str());
		group_name->SetShowHtml(true);
	}
	else
	{
		group_name->SetVisible(false);
		nick_name->SetVisible(true);
		nick_name->SetShowHtml(true);
		nick_name->SetText(html_text.c_str());
		nick_name->SetShowHtml(true);
	}


	if (item.folder)
	{
		pListElement->SetFixedHeight(kGroupItemHeight);		
	}
	else
	{
		pListElement->SetFixedHeight(kFriendListItemNormalHeight);		
	}
	pListElement->SetTag((UINT_PTR)node);

	//
	_stprintf_s(szBuf, MAX_PATH - 1, _T("%s%d"), STR_ORG_MEM_LIST_ITEM, item.itemId);
	pListElement->SetName(szBuf);

	int index = 0;
	if (parent->has_children())
	{
		Node* prev = parent->get_last_child();
		index = prev->data().list_elment_->GetIndex() + 1;
	}
	else 
	{
		if (parent == root_node_)
			index = 0;
		else
			index = parent->data().list_elment_->GetIndex() + 1;
	}
	if (!CListUI::AddAt(pListElement, index))
	{
		delete pListElement;
		delete node;
		node = NULL;
	}

	parent->add_child(node);
	return node;
}



//更新头像
void CUIOrgMembersList::UpdataAvatar(tstring& strImageFile, int index)
{
	CListContainerElementUI* pElem = static_cast<CListContainerElementUI*>(CListUI::GetItemAt(index));

	CControlUI* pBtnLogo = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pElem, kFriendAvatar));
	if (pBtnLogo != NULL)
	{
		if (!strImageFile.empty())
		{
			pBtnLogo->SetBkImage(strImageFile.c_str());
		}
	}
}

void CUIOrgMembersList::UpdateShowName(tstring& strShowName, int index)
{
	CListContainerElementUI* pElem = static_cast<CListContainerElementUI*>(CListUI::GetItemAt(index));

	CControlUI* pShowName = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pElem, kNickNameCtrlName));
	if (pShowName != NULL)
	{
		if (!strShowName.empty())
		{
			pShowName->SetText(strShowName.c_str());
		}
	}
}


//遍历List item，如果账号相同，就删除该账号。
void CUIOrgMembersList::RemoveItem(int nItemID)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CListContainerElementUI* pListItem = static_cast<CListContainerElementUI*>(GetItemAt(i));
			Node* pNode = (Node*)pListItem->GetTag();
			if (pNode->data().value == nItemID)
			{
				Remove(pListItem);
				break;
			}
		}
	}
}


int CALLBACK CUIOrgMembersList::CompareListItem(UINT_PTR lPara1, UINT_PTR lPara2, UINT_PTR dwSortType)
{

	CControlUI* pControl1 = (CControlUI*)lPara1;
	CControlUI* pControl2 = (CControlUI*)lPara2;
	if (NULL != pControl1 && NULL != pControl2)
	{
		tstring strName1 = pControl1->GetName();
		tstring strName2 = pControl2->GetName();
		tstring strText1 = pControl1->GetText();
		tstring strText2 = pControl2->GetText();
		Node* pNode1 = (Node*)pControl1->GetTag();
		Node* pNode2 = (Node*)pControl2->GetTag();

		int nItemID1 = pNode1->data().value;
		int nItemID2 = pNode2->data().value;
		tstring strT1 = pNode1->data().text_;
		tstring strT2 = pNode2->data().text_;

		return strcmpi(strT1.c_str(), strT2.c_str());
	}

	return 0;
}

void CUIOrgMembersList::Sort()
{
	int dwData = 0;
	SortItems(CUIOrgMembersList::CompareListItem, dwData);
}

void CUIOrgMembersList::DoEvent(TEventUI& event)
{
	CFriendListBaseUI::DoEvent(event);
}

void CUIOrgMembersList::DeSelectItem(int nItemID)
{
	int nCount = GetCount();
	for (int i=0; i<nCount; i++)
	{
		CListContainerElementUI* pListItem = static_cast<CListContainerElementUI*>(GetItemAt(i));
		Node* pNode = (Node*)pListItem->GetTag();
		if (pNode->data().value == nItemID)
		{
			COptionUI* pOption = static_cast<COptionUI*>(paint_manager_.FindSubControlByName(pListItem, "option_oppo_employe_selected"));
			if (NULL != pOption)
			{
				if (pOption->IsSelected())
				{
					pOption->Selected(false);
					break;
				}
			}
		}
	}
}