#include "stdafx.h"
#include "ListResPackets.h"
#include "Utility.h"
#include "Controller.h"
#include "..\include\Global_Def.h"
#include "mainDlg.h"
// namespace DuiLib
// {
	const int kPackQingyueHeight=120;
	const int kPackItemHeight	= 80;
	const int kFileItemHeight	= 50;

	const TCHAR* string_expand_icon = _T("PackList\\arrow1.png");
	const TCHAR* string_unexpand_icon = _T("PackList\\arrow2.png");
	const TCHAR* string_Un_qianyuePack = _T("PackList\\pack.png");
	const TCHAR* string_qianyuePack = _T("PackList\\package_red.png");

	#define STR_RESLIST_ITEM _T("ResListItem_")

	CListResPacket::CListResPacket(CPaintManagerUI& paint_manager)
		: root_node_(NULL)
		, delay_deltaY_(0)
		, delay_number_(0)
		, delay_left_(0)
 		, level_expand_image_(_T("<i PackList\\arrow2.png>"))
 		, level_collapse_image_(_T("<i PackList\\arrow1.png>"))
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

	CListResPacket::~CListResPacket()
	{
		if (root_node_)
			delete root_node_;

		root_node_ = NULL;
	}

	bool CListResPacket::Add(CControlUI* pControl)
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;

		return CListUI::Add(pControl);
	}

	bool CListResPacket::AddAt(CControlUI* pControl, int iIndex)
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;

		return CListUI::AddAt(pControl, iIndex);
	}

	bool CListResPacket::Remove(CControlUI* pControl)
	{
		if (!pControl)
			return false;

		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;

		return CListUI::Remove(pControl);
	}

	bool CListResPacket::RemoveAt(int iIndex)
	{
		CControlUI* pControl = GetItemAt(iIndex);
		if (!pControl)
			return false;

		tstring strClass = pControl->GetClass();
		if (_tcsicmp(pControl->GetClass(), _T("ListContainerElementUI")) != 0)
			return false;

		return CListUI::RemoveAt(iIndex);
	}

	void CListResPacket::RemoveAll()
	{
		CListUI::RemoveAll();
		delete root_node_;

		root_node_ = new Node;
		root_node_->data().level_ = -1;
		root_node_->data().child_visible_ = true;
		root_node_->data().has_child_ = true;
		root_node_->data().list_elment_ = NULL;
	}

	void CListResPacket::DoEvent(TEventUI& event) 
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

	Node* CListResPacket::GetRoot()
	{
		return root_node_;
	}

	void CListResPacket::RemoveNode(Node* node)
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


	void CListResPacket::RemoveChildNode(Node* node)
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

	void CListResPacket::SetChildVisible(Node* node, bool visible)
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
			//if (node->data().child_visible_)
			//	html_text += level_expand_image_;
			//else
			//	html_text += level_collapse_image_;
			CControlUI* pPackExpand = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(node->data().list_elment_, "gm_pack_expand"));
			pPackExpand->SetVisible(true);
			if (node->data().child_visible_)
			{
				pPackExpand->SetBkImage(string_expand_icon);
			}
			else
			{
				pPackExpand->SetBkImage(string_unexpand_icon);
			}
			


			_stprintf_s(szBuf, MAX_PATH - 1, _T("<x %d>"), level_text_start_pos_);
			html_text += szBuf;

			html_text += node->data().text_;

// 			CLabelUI* group_name = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(node->data().list_elment_, kGroupControlName));
// 			if (group_name != NULL)
// 			{
// 				group_name->SetShowHtml(true);
// 				group_name->SetText(html_text.c_str());
// 			}
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

	bool CListResPacket::CanExpand(Node* node) const
	{
		if (!node || node == root_node_)
			return false;

		return node->data().has_child_;
	}

	void CListResPacket::SetChildVisible(Node* pParentNode, UINT strValue, bool visible)
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

	void CListResPacket::ShowAllNode(Node* pParentNode, bool bVisible)
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



	bool CListResPacket::IsParentNode( Node* node )
	{
		return node->data().folder_;
	}

	Node* CListResPacket::GetChildNode( Node* pParentNode, UINT itemId )
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

	//对于签约资料包：4中，需要”同意/拒绝“按钮。
	Node* CListResPacket::AddNode(const FriendListItemInfoEx& item, Node* parent)
	{
		if (!parent)
			parent = root_node_;

		TCHAR szBuf[MAX_PATH] = {0};

		CListContainerElementUI* pListElement = NULL;
		if( !m_dlgBuilder.GetMarkup()->IsValid() ) {
			pListElement = static_cast<CListContainerElementUI*>(m_dlgBuilder.Create(_T("groupchat_filelist_item.xml"), (UINT)0, NULL, &paint_manager_));
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


		bool bQianyuePack = false;
		if (item.folder)
		{
			bQianyuePack = IsQingyuePack(tstring(item.strPackProp));
		}

		//资料包的属性:
		if (item.folder)
		{
			//如果是签约包，设置红色的图标。
			node->data().strPackIcon = bQianyuePack ? string_qianyuePack : string_Un_qianyuePack;
			node->data().strPackID = item.strPackID;
			node->data().strPackName = item.strPackName;
			node->data().strPackTime = item.strPackTime;
			node->data().strPackSender = item.strPackSender;
			node->data().strPackSize =  item.strPackSize;
			node->data().strPackProp = item.strPackProp;
			node->data().nPackStatus = item.nPackStatus;
			node->data().strPackCreaterPhone = item.strPackCreater;
		}
		else
		{
			tstring strFileType = item.strFileType;
			node->data().strPackID = item.strPackID;
			node->data().strPackName = item.strPackName;
			node->data().strFileIcon = GetPackFileIcon(strFileType);
			node->data().strFileName = item.strFileName;
			node->data().strFileSizeShow = item.strFileSize;
			node->data().strFileSizeAccurate = item.strFileSizeAccurate;
			node->data().strAliUrl = item.strFileUrl;
		}

		//显示 同意拒绝 的策略：
		bool bShowAgreeDeny = true; //
	
		tstring strMyOrgID = mainDlg::getInstance()->getMyselfItem().m_strOrgID;
		tstring strMyPhone = CController::Instance()->GetXmppClient()->jid().username();

		tstring strSenderOrg = item.strOrgID;
		//资料包的发送者和我的机构Id相同：判断是不是‘我的好友’
		if(_tcsicmp(strSenderOrg.c_str(), strMyOrgID.c_str()) == 0)
		{
			if ( _tcsicmp(item.strOrgID.c_str(), DEF_ORGID_MYFRIEND) == 0)
			{	
				//判断是不是我自己发送的
				if(_tcsicmp(item.strPackCreater.c_str(), strMyPhone.c_str()) == 0)
				{
					bShowAgreeDeny = false;
				}
				//是我的好友，但不是我自己。
				else
				{
					bShowAgreeDeny = true;
				}
			}
			else
			{
				bShowAgreeDeny = false;
			}
		}
		//该资料包发送者跟我不在一个组织里面
		else
		{
			bShowAgreeDeny = true;
		}

		if (item.folder)
		{
			SetPackState(pListElement, true,  &(node->data()));
			SetFileState(pListElement, false, &(node->data()));

			//如果是签约包。
			if (bQianyuePack)
			{
				//如果本方发送的或者是自己的发送的
				//if(_tcsicmp(item.strPackCreater.c_str(), strMyPhone.c_str()) == 0)
				if(!bShowAgreeDeny)
				{
					//隐藏同意，拒绝
					SetAgreeDenyBtnState(pListElement, false);

					//如果是 处理过的，并且”同意“的签约资料包
					if (item.nPackStatus == 3)
					{	
						SetQianyueState(pListElement, true, tstring(_T("已签约")));
					}
					//如果是 处理过的，并且”拒绝“的资料包
					else if(item.nPackStatus == 4)
					{
						SetQianyueState(pListElement, true, tstring(_T("已拒绝")));
					}
					//如果是未处理过的:默认，保存，发送
					else if (item.nPackStatus == 0 || item.nPackStatus == 1 || item.nPackStatus == 2)
					{
						SetQianyueState(pListElement, true, tstring(_T("未处理")));
					}
					
				}
				//别人发送的
				else
				{
					//如果是 处理过的，并且”同意“的签约资料包
					if (item.nPackStatus == 3)
					{
						//隐藏同意，拒绝
						SetAgreeDenyBtnState(pListElement, false);
						//设置签约状态
						SetQianyueState(pListElement, true, tstring(_T("已签约")));
					}
					//如果是 处理过的，并且”拒绝“的资料包
					else if(item.nPackStatus == 4)
					{
						//隐藏同意，拒绝
						SetAgreeDenyBtnState(pListElement, false);
						//设置签约状态
						SetQianyueState(pListElement, true, tstring(_T("已拒绝")));
					}
					//那么就是没处理的包了。。。显示同意，拒绝:默认，保存，发送
					else if(item.nPackStatus == 0 || item.nPackStatus == 1 || item.nPackStatus == 2)
					{
						//如果自己有执行的权限：
						if (m_bExcute)
						{
							//显示 同意，拒绝
							SetAgreeDenyBtnState(pListElement, true);	
							//隐藏状态
							SetQianyueState(pListElement, false, tstring(_T("")));

						}
						else //没有权限 同意拒绝。就显示 未处理。
						{
							//隐藏同意拒绝按钮，且显示当前状态为”未处理“=====>高度为80,下面的code会设置。
							SetAgreeDenyBtnState(pListElement, false);	
							//隐藏状态
							SetQianyueState(pListElement, false, tstring(_T("未处理")));
						}
					}
				}
			}
			else
			{
				//普通的数据包：隐藏->同意 拒绝 状态按钮
				SetAgreeDenyBtnState(pListElement, false);
				SetQianyueState(pListElement, false, tstring(_T("")));
			}
		}
		else
		{
			SetPackState(pListElement, false, &(node->data()));
			SetFileState(pListElement, true,  &(node->data()));
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

		if (item.folder)
		{
			if (bQianyuePack)
			{
				//默认，保存，发送
				if (item.nPackStatus == 0 || item.nPackStatus == 1 || item.nPackStatus == 2)
				{
					//如果是我发送的，且对方还没有处理的签约单：未处理。
					//if (_tcsicmp(item.strPackCreater.c_str(), strMyPhone.c_str()) == 0)
					if(!bShowAgreeDeny)
					{
						pListElement->SetFixedHeight(kPackItemHeight);		
					}
					else
					{
						//if 我有权限就显示为120高度。如果没有权限，就显示80高度。
						if (m_bExcute)
						{
							pListElement->SetFixedHeight(kPackQingyueHeight);	
						}
						else
						{
							pListElement->SetFixedHeight(kPackItemHeight);	
						}
						
					}
				}
				else if (item.nPackStatus == 3 ||  item.nPackStatus == 4)
				{
					pListElement->SetFixedHeight(kPackItemHeight);		
				}
			}
			else
			{
				pListElement->SetFixedHeight(kPackItemHeight);
			}
		}
		else
		{
			pListElement->SetFixedHeight(kFileItemHeight);		
		}
		pListElement->SetTag((UINT_PTR)node);

		//
		_stprintf_s(szBuf, MAX_PATH - 1, _T("%s%d"), STR_RESLIST_ITEM, item.itemId);
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
		int nndex = pListElement->GetIndex();
		parent->add_child(node);
		return node;
	}

	void CListResPacket::SetPackState(CListContainerElementUI* pListElement, bool bVisible, DuiLib::NodeData* pNodeData)
	{
		CControlUI* pPackIcon = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListElement, _T("gm_pack_dir_icon")));
		pPackIcon->SetVisible(bVisible);
		pPackIcon->SetBkImage(pNodeData->strPackIcon.c_str());

		CControlUI* pPackName = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListElement, _T("gm_pack_name")));
		pPackName->SetVisible(bVisible);
		pPackName->SetText(pNodeData->strPackName.c_str());

		CLabelUI* pPacktime = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, _T("gm_pack_time")));
		pPacktime->SetVisible(bVisible);
		pPacktime->SetText(pNodeData->strPackTime.c_str());

		CControlUI* pPackExpand = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListElement, _T("gm_pack_expand")));
		pPackExpand->SetVisible(bVisible);
		pPackExpand->SetBkImage(string_unexpand_icon);
		
		CLabelUI* pPacksend = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, _T("gm_pack_sender")));
		pPacksend->SetVisible(bVisible);
		tstring strSender(_T("发送人："));
		strSender += pNodeData->strPackSender;
		pPacksend->SetText(strSender.c_str());


		CLabelUI* pPacksize = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, _T("gm_pack_size")));
		pPacksize->SetVisible(bVisible);
		pPacksize->SetText(pNodeData->strPackSize.c_str());

		CLabelUI* pPackprop = static_cast<CLabelUI*>(paint_manager_.FindSubControlByName(pListElement, _T("gm_pack_propety")));
		pPackprop->SetVisible(bVisible);
		pPackprop->SetText(pNodeData->strPackProp.c_str());

		CControlUI* pPackBtnOpen = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListElement, _T("gm_button_openfile")));
		pPackBtnOpen->SetVisible(bVisible);
	}

	void CListResPacket::SetFileState(CListContainerElementUI* pListElement, bool bVisible, DuiLib::NodeData* pNodeData)
	{
		CControlUI* pFileIcon = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListElement, _T("gm_file_icon")));
		pFileIcon->SetVisible(bVisible);
		pFileIcon->SetBkImage(pNodeData->strFileIcon.c_str());

		CControlUI* pFileName = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListElement, _T("gm_file_name")));
		pFileName->SetVisible(bVisible);
		pFileName->SetText(pNodeData->strFileName.c_str());

		CControlUI* pFileSize = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListElement, _T("gm_file_size")));
		pFileSize->SetVisible(bVisible);
		pFileSize->SetText(pNodeData->strFileSizeShow.c_str());
	
		CControlUI* pPackBtnOpen = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListElement, _T("gm_button_openfile")));
		pPackBtnOpen->SetVisible(bVisible);
	}

	void CListResPacket::SetAgreeDenyBtnState(CListContainerElementUI* pListElement, bool bVisible)
	{
		CControlUI* pPackAgreeBtn = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListElement, _T("gm_button_agree")));
		if (NULL != pPackAgreeBtn)
		{
			pPackAgreeBtn->SetVisible(bVisible);
		}

		CControlUI* pPackDenyBtn = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListElement, _T("gm_button_deny")));
		if (NULL != pPackDenyBtn)
		{
			pPackDenyBtn->SetVisible(bVisible);
		}
	}
	void CListResPacket::SetQianyueState(CListContainerElementUI* pListItem, bool bVisible, tstring& strState)
	{
		//
		CControlUI* pQianyueState = static_cast<CControlUI*>(paint_manager_.FindSubControlByName(pListItem, _T("label_qianyue_state")));
		if (NULL != pQianyueState)
		{
			pQianyueState->SetVisible(bVisible);
			if (bVisible)
			{
				pQianyueState->SetText(strState.c_str());
			}
		}
	}

	void CListResPacket::UpdateQianyuePackState(tstring& strPackID, tstring& newState)
	{
		int nCount = GetCount();
		if (nCount>0)
		{
			for (int i=0; i<nCount; i++)
			{
				CListContainerElementUI* pListItem = static_cast<CListContainerElementUI*>(GetItemAt(i));
				if (NULL != pListItem)
				{
					Node* pNode = (Node*)pListItem->GetTag();
					if (NULL != pNode)
					{
						//只更新父亲节点。
						if (pNode->data().folder_ &&
							_tcsicmp(pNode->data().strPackID.c_str(), strPackID.c_str()) == 0)
						{
							//更新当前room的资料包列表：如果当前激活的不是这个room呢，
							SetAgreeDenyBtnState(pListItem, false);
							SetQianyueState(pListItem, true, newState /*== "3" ? tstring("已签约") : tstring("已拒绝")*/);
							pListItem->SetFixedHeight(80);
						}
					}
				}
			}
		}
	}