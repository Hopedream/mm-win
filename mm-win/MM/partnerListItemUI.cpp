#include "stdafx.h"
#include "partnerListItemUI.h"

namespace DuiLib
{

	const int kFriendListItemNormalHeight = 32;

	const TCHAR* const kLogoButtonControlName = _T("logo");
	const TCHAR* const kLogoContainerControlName = _T("logo_container");
	const TCHAR* const kCallIconContainerControlName = _T("callIconContainer");

	const TCHAR* const kNickNameControlName = _T("nickname");
	const TCHAR* const kOrgNameControlName = _T("orgname");
	const TCHAR* const kVedioCallBtnName = _T("btnLauchVedioCall");
	const TCHAR* const kVoiceCallBtnName = _T("btnLauchVoiceCall");


	CPartnerListItemUI::CPartnerListItemUI(CPaintManagerUI& paint_manager)
		: root_node_(NULL)
		, delay_deltaY_(0)
		, delay_number_(0)
		, delay_left_(0)
		, level_expand_image_(_T("<i list_icon_b.png>"))
		, level_collapse_image_(_T("<i list_icon_a.png>"))
		, level_text_start_pos_(10)
		, text_padding_(10, 0, 0, 0)
		, paint_manager_(paint_manager)
	{
		SetItemShowHtml(true);

		root_node_ = new Node;
		root_node_->data().level_ = -1;
		root_node_->data().child_visible_ = true;
		root_node_->data().has_child_ = true;
		root_node_->data().list_elment_ = NULL;
	}

	CPartnerListItemUI::~CPartnerListItemUI()
	{
		if (root_node_)
			delete root_node_;

		root_node_ = NULL;
	}

	bool CPartnerListItemUI::Add(CControlUI* pControl)
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) == 0)
			return false;

		return CListUI::Add(pControl);
	}

	bool CPartnerListItemUI::AddAt(CControlUI* pControl, int iIndex)
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) == 0)
			return false;

		return CListUI::AddAt(pControl, iIndex);
	}

	bool CPartnerListItemUI::Remove(CControlUI* pControl)
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) == 0)
			return false;

		return CListUI::Remove(pControl);
	}

	bool CPartnerListItemUI::RemoveAt(int iIndex)
	{
		CControlUI* pControl = GetItemAt(iIndex);
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) == 0)
			return false;

		return CListUI::RemoveAt(iIndex);
	}

	void CPartnerListItemUI::RemoveAll()
	{
		CListUI::RemoveAll();
		delete root_node_;

		root_node_ = new Node;
		root_node_->data().level_ = -1;
		root_node_->data().child_visible_ = true;
		root_node_->data().has_child_ = true;
		root_node_->data().list_elment_ = NULL;
	}

	void CPartnerListItemUI::DoEvent(TEventUI& event) 
	{
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

	Node* CPartnerListItemUI::GetRoot()
	{
		return root_node_;
	}

	Node* CPartnerListItemUI::AddNode(const FriendListItemInfo& item, Node* parent)
	{
		if (!parent)
			parent = root_node_;

		TCHAR szBuf[MAX_PATH] = {0};

		CListContainerElementUI* pListElement = NULL;
		if( !m_dlgBuilder.GetMarkup()->IsValid() ) {
			pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create(_T("partner\\partnerLisItem.xml"), (UINT)0, NULL, &paint_manager_));
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

		node->data().text_ = item.strOrgName;
		node->data().value = item.id;
		node->data().list_elment_ = pListElement;

		if (!parent->data().child_visible_)
			pListElement->SetVisible(false);

		if (parent != root_node_ && !parent->data().list_elment_->IsVisible())
			pListElement->SetVisible(false);

		CDuiRect rcPadding = text_padding_;
		for (int i = 0; i < node->data().level_; ++i)
		{
			rcPadding.left += level_text_start_pos_;		
		}
		pListElement->SetPadding(rcPadding);

		CControlUI* log_button = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListElement, kLogoButtonControlName));
		if (log_button != NULL)
		{
			if (!item.folder && !item.logo.empty())
			{
				log_button->SetBkImage(item.logo.c_str());
			}
			else
			{
				CContainerUI* logo_container = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pListElement, kLogoContainerControlName));
				CContainerUI* call_container = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pListElement, kCallIconContainerControlName));
				if (logo_container != NULL) logo_container  ->SetVisible(false);
				if (call_container != NULL) call_container  ->SetVisible(false);
			}
		}

		string html_text;
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
			//_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), item.nick_name.c_str());
			_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), item.strOrgName.c_str());
			html_text += szBuf;
		}

		CLabelUI* orgNameLabel = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kOrgNameControlName));
		if (orgNameLabel != NULL)
		{
			if (item.folder)
				orgNameLabel->SetFixedWidth(0);

			orgNameLabel->SetShowHtml(true);
			orgNameLabel->SetText(html_text.c_str());
		}

		if (!item.folder && !item.nick_name.empty())
		{
			CLabelUI* nick_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kNickNameControlName));
			if (nick_name != NULL)
			{
				_stprintf_s(szBuf, MAX_PATH - 1, _T("<x 20><c #808080><f 2>%s</c></f>"), item.nick_name.c_str());
				nick_name->SetShowHtml(true);
				nick_name->SetText(item.nick_name.c_str());
			}
		}

		pListElement->SetFixedHeight(kFriendListItemNormalHeight);
		pListElement->SetTag((UINT_PTR)node);

		CButtonUI* pBtnVoiceCall= static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pListElement, kVoiceCallBtnName));
		CButtonUI* pBtnVedioCall= static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pListElement, kVedioCallBtnName));
		if (NULL != pBtnVoiceCall)  pBtnVoiceCall->SetTag((UINT_PTR)node);
		if (NULL != pBtnVedioCall)  pBtnVedioCall->SetTag((UINT_PTR)node);

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

	void CPartnerListItemUI::RemoveNode(Node* node)
	{
		if (!node || node == root_node_) return;

		for (int i = 0; i < node->num_children(); ++i)
		{
			Node* child = node->child(i);
			RemoveNode(child);
		}

		CListUI::Remove(node->data().list_elment_);
		delete node->data().list_elment_;
		node->parent()->remove_child(node);
		delete node;
	}

	void CPartnerListItemUI::SetChildVisible(Node* node, bool visible)
	{
		if (!node || node == root_node_)
			return;

		if (node->data().child_visible_ == visible)
			return;

		node->data().child_visible_ = visible;

		TCHAR szBuf[MAX_PATH] = {0};
		string html_text;
		if (node->data().has_child_)
		{
			if (node->data().child_visible_)
				html_text += level_expand_image_;
			else
				html_text += level_collapse_image_;

			_stprintf_s(szBuf, MAX_PATH - 1, _T("<x %d>"), level_text_start_pos_);
			html_text += szBuf;

			html_text += node->data().text_;

			CLabelUI* org_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(node->data().list_elment_, kOrgNameControlName));
			if (org_name != NULL)
			{
				org_name->SetShowHtml(true);
				org_name->SetText(html_text.c_str());
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
					if (local_parent->data().child_visible_ && local_parent->data().list_elment_->IsVisible())
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

	bool CPartnerListItemUI::CanExpand(Node* node) const
	{
		if (!node || node == root_node_)
			return false;

		return node->data().has_child_;
	}

} // namespace DuiLib