#ifndef UI_FRIENDS_LIST_BASE_H_H_
#define UI_FRIENDS_LIST_BASE_H_H_

#include "UIListCommonDefine.h"

namespace DuiLib
{
struct FriendListItemInfo
{
	bool folder;
	bool empty;
	UINT itemId;
	tstring logo;
	tstring strOrgName;
	tstring strOrgID;
	tstring nick_name;
	tstring account;//added by zyyΪ�˵õ���friend�����ԣ���Ҫ�Ӵ��ֶΣ����ܻᵼ��һЩδ���ֵ����⡣Ĭ����Ϊ��
	FriendListItemInfo(bool _folder = false, bool _empty = false, UINT _itemId = 0, tstring _account = (_T("")), tstring _logo = (_T("")), tstring _strOrgName  = (_T("")), tstring _nick_name  = (_T("")))
	{
		folder = _folder;
		empty = _empty;
		itemId = _itemId;
		account = _account;
		logo = _logo;
		strOrgName = _strOrgName;
		nick_name = _nick_name;
	}
};


class CFriendListBaseUI : public CListUI
{
public:
	enum {SCROLL_TIMERID = 10};

	CFriendListBaseUI(CPaintManagerUI& paint_manager);

	~CFriendListBaseUI();

	bool Add(CControlUI* pControl);

	bool AddAt(CControlUI* pControl, int iIndex);

	bool Remove(CControlUI* pControl);

	bool RemoveAt(int iIndex);

	void RemoveAll();

	void DoEvent(TEventUI& event);

	Node* GetRoot();

	virtual Node* AddNode(const FriendListItemInfo& item, Node* parent = NULL) = 0;

	void RemoveNode(Node* node);
	bool IsParentNode(Node* node);
	void SetChildVisible(Node* node, bool visible);
	void SetChildVisible(Node* pParentNode, UINT strValue, bool visible = true);
	bool CanExpand(Node* node) const;
	void RemoveChildNode(Node* node);
	Node* GetChildNode(Node* pParentNode, UINT itemId); //ͨ�����ڵ���ӽڵ��itemid��ѯ�ӽڵ�
	void CFriendListBaseUI::ShowAllNode(Node* pParentNode, bool bVisible = true);
	//bool SelectItem(int iIndex);

protected:
	Node*	root_node_;
	LONG	delay_deltaY_;
	DWORD	delay_number_;
	DWORD	delay_left_;
	CDuiRect	text_padding_;
	int level_text_start_pos_;
	tstring level_expand_image_;
	tstring level_collapse_image_;
	CPaintManagerUI& paint_manager_;

    CDialogBuilder m_dlgBuilder;
};

} // DuiLib

#endif // UI_FRIENDS_LIST_BASE_H_H_