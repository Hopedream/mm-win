#ifndef UIFRIENDS_HPP
#define UIFRIENDS_HPP

#include "UIListCommonDefine.h"

namespace DuiLib
{

class CPartnerListItemUI : public CListUI
{
public:
	enum {SCROLL_TIMERID = 10};

	CPartnerListItemUI(CPaintManagerUI& paint_manager);

	~CPartnerListItemUI();

	bool Add(CControlUI* pControl);

	bool AddAt(CControlUI* pControl, int iIndex);

	bool Remove(CControlUI* pControl);

	bool RemoveAt(int iIndex);

	void RemoveAll();

	void DoEvent(TEventUI& event);

	Node* GetRoot();

	Node* AddNode(const FriendListItemInfo& item, Node* parent = NULL);

	void RemoveNode(Node* node);

	void SetChildVisible(Node* node, bool visible);

	bool CanExpand(Node* node) const;

	//bool SelectItem(int iIndex);

private:
	Node*	root_node_;
	LONG	delay_deltaY_;
	DWORD	delay_number_;
	DWORD	delay_left_;
	CDuiRect	text_padding_;
	int level_text_start_pos_;
	string level_expand_image_;
	string level_collapse_image_;
	CPaintManagerUI& paint_manager_;

    CDialogBuilder m_dlgBuilder;
};

} // DuiLib

#endif // UIFRIENDS_HPP