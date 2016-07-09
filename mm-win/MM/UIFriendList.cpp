#include "UIFriendList.h"
#include <sstream>
#include "Utility.h"

const int kFriendListItemNormalHeight	= 40;
const int kGroupItemHeight	= 30;

const TCHAR* const kFriendAvatar		= _T("friendAvatar");		//好友头像
const TCHAR* const kLogoContainerControlName			= _T("logo_container");			
const TCHAR* const kGroupCtrlName	= _T("groupName");	
const TCHAR* const kNickNameCtrlName	= _T("friendName");	

CUIFriendList::CUIFriendList( CPaintManagerUI* paint_manager )
	:CFriendListBaseUI(*paint_manager)
{
}



Node* CUIFriendList::AddNode(const FriendListItemInfo& item, Node* parent)
{
	if (!parent)
		parent = root_node_;

	TCHAR szBuf[MAX_PATH] = {0};

	CListContainerElementUI* pListElement = NULL;
	if( !m_dlgBuilder.GetMarkup()->IsValid() ) {
		pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create(_T("friend_list_item.xml"), (UINT)0, NULL, &paint_manager_));
	}
	else {
		pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create((UINT)0, &paint_manager_));
	}
	if (pListElement == NULL)
		return NULL;

	Node* node = new Node;

	node->data().level_ = parent->data().level_ + 1;
	if (item.folder)
		node->data().has_child_ = !item.empty;
	else
		node->data().has_child_ = false;

	node->data().folder_ = item.folder;

	node->data().child_visible_ = (node->data().level_ == 0);
	node->data().child_visible_ = false;

	node->data().strOrgID = item.strOrgID;//新增的机构ID
	node->data().text_ = item.strOrgName;
	node->data().value = item.itemId;
	node->data().list_elment_ = pListElement;

	if (!parent->data().child_visible_)
		pListElement->SetVisible(false);

	if (parent != root_node_ && !parent->data().list_elment_->IsVisible())
		pListElement->SetVisible(false);

	//CDuiRect rcPadding = text_padding_;
	//for (int i = 0; i < node->data().level_; ++i)
	//{
	//	rcPadding.left += level_text_start_pos_;		
	//}
	//pListElement->SetPadding(rcPadding);

	CControlUI* log_button = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListElement, kFriendAvatar));
	if (log_button != NULL)
	{
		if (!item.folder && !item.logo.empty())
		{
			log_button->SetBkImage(item.logo.c_str());
		}
		else
		{
			CContainerUI* logo_container = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pListElement, kLogoContainerControlName));
			if (logo_container != NULL) logo_container  ->SetVisible(false);
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
		//_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), item.strOrgName.c_str());
		html_text += szBuf;
	}

	CLabelUI* nick_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kNickNameCtrlName));

	CLabelUI* group_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kGroupCtrlName));


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

	//属性显示
	//2.设置属性
	vector<CLabelUI*> vect_label;
	vector<tstring> vecDuty;
	tstring s1 = item.strOrgID;
	tstring s2 = item.account;
	GetDuty(item.strOrgID, item.account, vecDuty);	

	for (int i=1;i<=4;i++)
	{
		std::ostringstream ostr;
		ostr << "friend_duty_" << i;
		CLabelUI *pLabelProperty = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, ostr.str().c_str()));
		if (NULL != pLabelProperty)
		{
			if (i>vecDuty.size())
			{
				pLabelProperty->SetVisible(false);
			}
			else
			{
				pLabelProperty->SetText(vecDuty[i-1].c_str());
				pLabelProperty->SetVisible(true);
				vect_label.push_back(pLabelProperty);
			}
		}
	}

	//位置移动。。。。。。。。。。。。by zyy
	//duilib无法获取控件位置，可以设置控件位置，因此，这里的位置是从xml里面得到
	//得到名字占用的位置
	RECT rect_origin, rect_total;
	rect_origin.left = 36;
	rect_origin.right = 121;
	rect_origin.top = 10;
	rect_origin.bottom = 30;

	rect_total.left = 35;
	rect_total.right = 192;
	rect_total.top = 5;
	rect_total.bottom = 20;

	//得到名字字符串的长度
	HDC hdc = paint_manager_.GetPaintDC();
	SIZE sz = CRenderEngine::GetTextSize(hdc, &paint_manager_, nick_name->GetText(), -1,0);
	sz.cx += 12;

//	nick_name->SetPos(rect_origin);
	MoveControllers(nick_name, rect_origin, sz, vect_label, rect_total);
	//////////////////////////////////////////////////////////////////////////


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
	_stprintf_s(szBuf, MAX_PATH - 1, _T("%s%d"), STR_FRIENDLIST_ITEM, item.itemId);
	pListElement->SetName(szBuf);
//	pListElement->SetName("赵有元测试");

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
void CUIFriendList::UpdataAvatar(tstring& strImageFile, int index)
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

void CUIFriendList::UpdateShowName(tstring& strShowName, int index)
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

void CUIFriendList::UpdateDuty(const vector<tstring>& duty, int index)
{
	CListContainerElementUI* pElem = static_cast<CListContainerElementUI*>(CListUI::GetItemAt(index));

	CLabelUI* nick_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pElem, kNickNameCtrlName));
	vector<CLabelUI*> vect_label;
	for (int i=1;i<=4;i++)
	{
		std::ostringstream ostr;
		ostr << "friend_duty_" << i;
		CLabelUI *pLabelProperty = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pElem, ostr.str().c_str()));
		if (NULL != pLabelProperty)
		{
			if (i>duty.size())
			{
				pLabelProperty->SetVisible(false);
			}
			else
			{
				pLabelProperty->SetText(duty[i-1].c_str());
				pLabelProperty->SetVisible(true);
				vect_label.push_back(pLabelProperty);
			}
		}
	}

	//位置移动。。。。。。。。。。。。by zyy
	//duilib无法获取控件位置，可以设置控件位置，因此，这里的位置是从xml里面得到
	//得到名字占用的位置
	RECT rect_origin, rect_total;
	rect_origin.left = 36;
	rect_origin.right = 121;
	rect_origin.top = 10;
	rect_origin.bottom = 30;

	rect_total.left = 35;
	rect_total.right = 200;
	rect_total.top = 5;
	rect_total.bottom = 20;

	//得到名字字符串的长度
	HDC hdc = paint_manager_.GetPaintDC();
	SIZE sz = CRenderEngine::GetTextSize(hdc, &paint_manager_, nick_name->GetText(), -1,0);
	sz.cx += 12;

	//	nick_name->SetPos(rect_origin);
	MoveControllers(nick_name, rect_origin, sz, vect_label, rect_total);
	//////////////////////////////////////////////////////////////////////////
}


//遍历List item，如果账号相同，就删除该账号。
void CUIFriendList::RemoveItem(int nItemID)
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

void CUIFriendList::ModifyOrgName( Node* pNode, const tstring strOrgName )
{
	CListContainerElementUI* pListElement = pNode->data().list_elment_;
	pNode->data().text_ = strOrgName;
	CControlUI* pOrgNameCtrl = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListElement, kGroupCtrlName));
	CDuiString strText = pOrgNameCtrl->GetText();
	pOrgNameCtrl->SetText(strText.Left(strText.ReverseFind('>') + 1) + strOrgName.c_str());
}


