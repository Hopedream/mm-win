#include "stdafx.h"
#include "UIFriendListBase.h"

namespace DuiLib
{
	const int kFriendListItemNormalHeight = 40;
	const TCHAR* const kGroupControlName = _T("groupName");


	CFriendListBaseUI::CFriendListBaseUI(CPaintManagerUI& paint_manager)
		: root_node_(NULL)
		, delay_deltaY_(0)
		, delay_number_(0)
		, delay_left_(0)
		, level_expand_image_(_T("<i list_icon_b.png>"))
		, level_collapse_image_(_T("<i list_icon_a.png>"))
		, level_text_start_pos_(10)
		, text_padding_(0, 0, 0, 0)
		, paint_manager_(paint_manager)
	{
		SetItemShowHtml(true);

		root_node_ = new Node;
		root_node_->data().level_ = -1;
		root_node_->data().child_visible_ = true;
		root_node_->data().has_child_ = true;
		root_node_->data().list_elment_ = NULL;
	}

	CFriendListBaseUI::~CFriendListBaseUI()
	{
		if (root_node_)
			delete root_node_;

		root_node_ = NULL;
	}

	bool CFriendListBaseUI::Add(CControlUI* pControl)
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;

		return CListUI::Add(pControl);
	}

	bool CFriendListBaseUI::AddAt(CControlUI* pControl, int iIndex)
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;

		return CListUI::AddAt(pControl, iIndex);
	}

	bool CFriendListBaseUI::Remove(CControlUI* pControl)
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;

		return CListUI::Remove(pControl);
	}

	bool CFriendListBaseUI::RemoveAt(int iIndex)
	{
		CControlUI* pControl = GetItemAt(iIndex);
		if (!pControl)
			return false;

		tstring strClass = pControl->GetClass();
		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;

		return CListUI::RemoveAt(iIndex);
	}

	void CFriendListBaseUI::RemoveAll()
	{
		CListUI::RemoveAll();
		delete root_node_;

		root_node_ = new Node;
		root_node_->data().level_ = -1;
		root_node_->data().child_visible_ = true;
		root_node_->data().has_child_ = true;
		root_node_->data().list_elment_ = NULL;
	}

	void CFriendListBaseUI::DoEvent(TEventUI& event) 
	{
		if (event.Type == UIEVENT_CONTEXTMENU)
		{
			return;
		}
		if (event.Type == UIEVENT_RBUTTONUP )
		{
			m_pManager->SendNotify(this, _T("rclick"));
		}
		else if (event.Type == UIEVENT_DBLCLICK)
		{
			m_pManager->SendNotify(this, _T("dbclick"));
		}
		else if (event.Type == UIEVENT_BUTTONDOWN)
		{
			m_pManager->SendNotify(this, _T("itemclick"));
		}

		if (!IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND)
		{
			if (m_pParent != NULL)
				m_pParent->DoEvent(event);
			else
				CVerticalLayoutUI::DoEvent(event);
			return;
		}

		if (event.Type == UIEVENT_TIMER && event.wParam == SCROLL_TIMERID)
		{
			if (delay_left_ > 0)
			{
				--delay_left_;
				SIZE sz = GetScrollPos();
				LONG lDeltaY =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaY_);
				if ((lDeltaY > 0 && sz.cy != 0)  || (lDeltaY < 0 && sz.cy != GetScrollRange().cy ))
				{
					sz.cy -= lDeltaY;
					SetScrollPos(sz);
					return;
				}
			}
			delay_deltaY_ = 0;
			delay_number_ = 0;
			delay_left_ = 0;
			m_pManager->KillTimer(this, SCROLL_TIMERID);
			return;
		}
		if (event.Type == UIEVENT_SCROLLWHEEL)
		{
			LONG lDeltaY = 0;
			if (delay_number_ > 0)
				lDeltaY =  (LONG)(CalculateDelay((double)delay_left_ / delay_number_) * delay_deltaY_);
			switch (LOWORD(event.wParam))
			{
			case SB_LINEUP:
				if (delay_deltaY_ >= 0)
					delay_deltaY_ = lDeltaY + 8;
				else
					delay_deltaY_ = lDeltaY + 12;
				break;
			case SB_LINEDOWN:
				if (delay_deltaY_ <= 0)
					delay_deltaY_ = lDeltaY - 8;
				else
					delay_deltaY_ = lDeltaY - 12;
				break;
			}
			if
				(delay_deltaY_ > 100) delay_deltaY_ = 100;
			else if
				(delay_deltaY_ < -100) delay_deltaY_ = -100;

			delay_number_ = (DWORD)sqrt((double)abs(delay_deltaY_)) * 5;
			delay_left_ = delay_number_;
			m_pManager->SetTimer(this, SCROLL_TIMERID, 50U);
			return;
		}

		CListUI::DoEvent(event);
	}

	Node* CFriendListBaseUI::GetRoot()
	{
		return root_node_;
	}

	void CFriendListBaseUI::RemoveNode(Node* node)
	{
		if (!node || node == root_node_) return;
		int count = node->num_children();
		for (int i = 0; i < count; ++i)
		{
			Node* child = node->child(i);
			RemoveNode(child);
		}

		CListUI::Remove(node->data().list_elment_);
		//delete node->data().list_elment_;
		node->parent()->remove_child(node);
		delete node;
	}


	void CFriendListBaseUI::RemoveChildNode(Node* node)
	{
		if (!node || node == root_node_) return;
		int count = node->num_children();
		for (int i = 0; i < count; ++i)
		{
			Node* child = node->child(i);
			CListUI::Remove(child->data().list_elment_);
			delete child;
		}
		node->removeAllChild();
	}

	void CFriendListBaseUI::SetChildVisible(Node* node, bool visible)
	{
		if (!node || node == root_node_)
			return;

		if (node->data().child_visible_ == visible)
			return;

		node->data().child_visible_ = visible;

		TCHAR szBuf[MAX_PATH] = {0};
		tstring html_text;
		if (node->data().has_child_)
		{
			if (node->data().child_visible_)
				html_text += level_expand_image_;
			else
				html_text += level_collapse_image_;

			_stprintf_s(szBuf, MAX_PATH - 1, _T("<x %d>"), level_text_start_pos_);
			html_text += szBuf;

			html_text += node->data().text_;

			CLabelUI* group_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(node->data().list_elment_, kGroupControlName));
			if (group_name != NULL)
			{
				group_name->SetShowHtml(true);
				group_name->SetText(html_text.c_str());
			}
		}

		if (!node->data().list_elment_->IsVisible())
			return;

		if (!node->has_children())
			return;

		Node* begin = node->child(0);
		Node* end = node->get_last_child();
		for (int i = begin->data().list_elment_->GetIndex(); i <= end->data().list_elment_->GetIndex(); ++i)
		{
			CControlUI* control = GetItemAt(i);
			if (_tcsicmp(control->GetClass(), _T("ListContainerElementUI")) == 0)
			{
				if (visible) 
				{
					Node* local_parent = ((Node*)control->GetTag())->parent();
					if (local_parent->data().child_visible_ 
						&& local_parent->data().list_elment_ != NULL 
						&& local_parent->data().list_elment_->IsVisible())
					{
						control->SetVisible(true);
					}
				}
				else
				{
					control->SetVisible(false);
				}
			}
		}
	}

	bool CFriendListBaseUI::CanExpand(Node* node) const
	{
		if (!node || node == root_node_)
			return false;

		return node->data().has_child_;
	}

	void CFriendListBaseUI::SetChildVisible(Node* pParentNode, UINT strValue, bool visible)
	{
		if (!pParentNode || !pParentNode->has_children() )
			return;

		/*TCHAR szBuf[MAX_PATH] = {0};
		tstring html_text;
		if (pParentNode->data().has_child_)
		{
		html_text += level_expand_image_;
		_stprintf_s(szBuf, MAX_PATH - 1, _T("<x %d>"), level_text_start_pos_);
		html_text += szBuf;
		html_text += pParentNode->data().text_;
		CLabelUI* org_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pParentNode->data().list_elment_, kGroupControlName));
		if (org_name != NULL)
		{
		org_name->SetShowHtml(true);
		org_name->SetText(html_text.c_str());
		}
		}*/

		for (int idx = 0; idx < pParentNode->num_children(); ++idx)
		{
			Node* pChildNode = pParentNode->child(idx);
			if (strValue == pChildNode->data().value)
			{
				int itemIdx = pChildNode->data().list_elment_->GetIndex();
				CControlUI* control = GetItemAt(itemIdx);
				control->SetVisible(true);
			}
		}	
	}

	void CFriendListBaseUI::ShowAllNode(Node* pParentNode, bool bVisible)
	{
		if (!pParentNode)
			return;		

		GetItemAt(pParentNode->data().list_elment_->GetIndex())->SetVisible(bVisible);

		for (int idx = 0; idx < pParentNode->num_children(); ++idx)
		{
			Node* pChildNode = pParentNode->child(idx);
			int itemIdx = pChildNode->data().list_elment_->GetIndex();
			CControlUI* control = GetItemAt(itemIdx);
			control->SetVisible(bVisible);
		}	
	}



	bool CFriendListBaseUI::IsParentNode( Node* node )
	{
		return node->data().folder_;
	}

	Node* CFriendListBaseUI::GetChildNode( Node* pParentNode, UINT itemId )
	{
		if (NULL == pParentNode) return NULL;
		
		for (int idx = 0; idx < pParentNode->num_children(); ++idx)
		{
			Node* child = pParentNode->child(idx);
			if (child->data().value == itemId)
			{
				return child;
			}
		}
		return NULL;
	}


} // namespace DuiLib
