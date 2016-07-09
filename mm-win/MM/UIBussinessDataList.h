#pragma once
#include "UIListCommonDefine.h"
#include <list>
#include "UIDefine.h"
using namespace std;

struct BDLstItemInfo
{
	bool folder;
	bool empty ;
	bool isFavorite;
	UINT itemId;
	tstring strProName;
	tstring strAttribute;
	tstring strCollect;
	tstring strTime;
	tstring strStatus;
	tstring strPackId;
	tstring strPackName;
	tstring strPackSender;

	tstring strFileFullName;
	tstring strFileType;
	tstring strSize;
	tstring strUrl;
	tstring strFileList;
	vector<fileInfo> m_vecFiles;
	BDLstItemInfo(bool _folder = false, 
		bool _empty = false, 
		bool isFavorite = false,
		UINT _itemId = 0)
	{
		folder = _folder;
		empty = _empty;
		itemId = _itemId;
	}
};


class CUIBussinessDataList : public CListUI
{
public:
	CUIBussinessDataList(CPaintManagerUI& paint_manager);
	~CUIBussinessDataList(void);

public:
	enum {SCROLL_TIMERID = 101};

	bool Add(CControlUI* pControl);

	bool AddAt(CControlUI* pControl, int iIndex);

	bool Remove(CControlUI* pControl);

	bool RemoveAt(int iIndex);

	void RemoveAll();

	void DoEvent(TEventUI& event);

	Node* GetRoot();

	virtual Node* AddNode(const BDLstItemInfo& item, Node* parent = NULL);

	void RemoveNode(Node* node);
	bool IsParentNode(Node* node);
	void SetChildVisible(Node* node, bool visible);
	void SetChildVisible(Node* pParentNode, UINT strValue, bool visible = true);
	bool CanExpand(Node* node) const;
	void RemoveChildNode(Node* node);
	Node* GetChildNode(Node* pParentNode, UINT itemId); //通过父节点和子节点的itemid查询子节点
	void ShowAllNode(Node* pParentNode, bool bVisible = true);
	
	/*void SetPackState(CListContainerElementUI* pListElement, bool bVisible, DuiLib::NodeData* pNodeData);
	void SetFileState(CListContainerElementUI* pListElement, bool bVisible, DuiLib::NodeData* pNodeData);
	void SetAgreeDenyBtnState(CListContainerElementUI* pListElement, bool bVisible);
	void SetQianyueState(CListContainerElementUI* pListItem, bool bVisible, tstring& strState);

	void UpdateQianyuePackState(tstring& strPackID, tstring& newState);*/

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
	
};

