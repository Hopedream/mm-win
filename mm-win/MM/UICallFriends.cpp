#include "stdafx.h"
#include "UICallFriends.h"

namespace DuiLib
{

	const int kFriendListItemNormalHeight = 40;

	const TCHAR* const kLogoButtonControlName = _T("logo");
	const TCHAR* const kLogoContainerControlName = _T("logo_container");
	const TCHAR* const kCallIconContainerControlName = _T("callIconContainer");
	const TCHAR* const kGroupControlName = _T("groupName");

	const TCHAR* const kNickNameControlName = _T("nickname");
	const TCHAR* const kOrgNameControlName = _T("orgname");
	const TCHAR* const kVedioCallBtnName = _T("VoIPCall");
	const TCHAR* const kVoiceCallBtnName = _T("smartPhoneCall");


	CCallFriendsUI::CCallFriendsUI(CPaintManagerUI& _paint_manager): 	CFriendListBaseUI(_paint_manager)
	{
	}

	CCallFriendsUI::~CCallFriendsUI()
	{
	}


	Node* CCallFriendsUI::AddNode(const FriendListItemInfo& item, Node* parent)
	{
		if (!parent)
			parent = root_node_;

		TCHAR szBuf[MAX_PATH] = {0};

		CListContainerElementUI* pListElement = NULL;
		if( !m_dlgBuilder.GetMarkup()->IsValid() ) {
			pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create(_T("Call\\call_friend_list_item.xml"), (UINT)0, NULL, &paint_manager_));
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
			_stprintf_s(szBuf, MAX_PATH - 1, _T("<f 2><c #808080>%s</f></c>"), item.strOrgName.c_str());
			//_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), item.strOrgName.c_str());
			html_text += szBuf;
		}

		CLabelUI* orgNameLabel = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kOrgNameControlName));
		CLabelUI* nick_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kNickNameControlName));
		
		CLabelUI* group_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, kGroupControlName));
		CContainerUI* group_container = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pListElement, "VLayouGroup_container"));
		CContainerUI* friend_container = static_cast<CContainerUI*>(paint_manager_.FindSubControlByName(pListElement, "VLayouFriends_container"));		

		if (item.folder)
		{
			group_container->SetVisible(true);
			friend_container->SetVisible(false);
			group_name->SetText(html_text.c_str());
			group_name->SetShowHtml(true);
		}
		else
		{
			group_container->SetVisible(false);
			friend_container->SetVisible(true);
			orgNameLabel->SetShowHtml(true);
			orgNameLabel->SetText(html_text.c_str());
			orgNameLabel->SetShowHtml(true);
		}

		if (!item.folder && !item.nick_name.empty())
		{		
			tstring strNickName = item.nick_name.substr(0, min(item.nick_name.length(), 15));			
			_stprintf_s(szBuf, MAX_PATH - 1, _T("%s"), strNickName.c_str());
			nick_name->SetShowHtml(true);
			nick_name->SetText(szBuf);			
		}

		if (item.folder)
		{
			pListElement->SetFixedHeight(30);		
		}
		else
		{
			pListElement->SetFixedHeight(40);		
		}
		pListElement->SetTag((UINT_PTR)node);

		CButtonUI* pBtnVoiceCall= static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pListElement, kVoiceCallBtnName));
		CButtonUI* pBtnVedioCall= static_cast<CButtonUI*>(paint_manager_.FindSubControlByName(pListElement, kVedioCallBtnName));
		if (NULL != pBtnVoiceCall)  pBtnVoiceCall->SetTag(item.itemId);
		if (NULL != pBtnVedioCall)  pBtnVedioCall->SetTag(item.itemId);

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



} // namespace DuiLib
