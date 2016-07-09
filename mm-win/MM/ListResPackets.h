#pragma once
#include "UIListCommonDefine.h"
#include <list>
#include "UIDefine.h"

using namespace std;
//namespace DuiLib
//{
	struct FriendListItemInfoEx
	{
		bool folder;
		bool empty;
		UINT itemId;
		tstring logo;
		tstring strOrgName;
		tstring strOrgID;
		tstring nick_name;

		//pack info
		tstring strPackID;
		tstring strPackName;
		tstring strPackIcon;
		tstring strPackTime;
		tstring strPackSender;
		tstring strPackSize;
		tstring strPackProp;
		tstring strPackCreater;//phone number
		int    nPackStatus;
		//tstring strPackStatus;

		//file info
		tstring strFileIcon;
		tstring strFileName;
		tstring strFileSize;//显示的大小：1.2MB;
		tstring strFileSizeAccurate;
		tstring strFileType;
		tstring strFileUrl;

		FriendListItemInfoEx(bool _folder = false, 
			bool _empty = false, 
			UINT _itemId = 0, 
			tstring _logo=(_T("")), 
			tstring _strOrgName =(_T("")),
			tstring _nick_name =(_T("")))
		{
			folder = _folder;
			empty = _empty;
			itemId = _itemId;
			logo = _logo;
			strOrgName = _strOrgName;
			nick_name = _nick_name;
		}
	};


	class CListResPacket : public CListUI
	{
	public:
		enum {SCROLL_TIMERID = 101};

		CListResPacket(CPaintManagerUI& paint_manager);

		~CListResPacket();

		bool Add(CControlUI* pControl);

		bool AddAt(CControlUI* pControl, int iIndex);

		bool Remove(CControlUI* pControl);

		bool RemoveAt(int iIndex);

		void RemoveAll();

		void DoEvent(TEventUI& event);

		Node* GetRoot();

		void SetExcuteDuty(bool bExcute)
		{
			m_bExcute = true;
			//m_bExcute = bExcute;//需求待定，目前只要是对方都有权限执行。
		}
		virtual Node* AddNode(const FriendListItemInfoEx& item, Node* parent = NULL);

		void RemoveNode(Node* node);
		bool IsParentNode(Node* node);
		void SetChildVisible(Node* node, bool visible);
		void SetChildVisible(Node* pParentNode, UINT strValue, bool visible = true);
		bool CanExpand(Node* node) const;
		void RemoveChildNode(Node* node);
		Node* GetChildNode(Node* pParentNode, UINT itemId); //通过父节点和子节点的itemid查询子节点
		void ShowAllNode(Node* pParentNode, bool bVisible = true);
		//bool SelectItem(int iIndex);
		void SetPackState(CListContainerElementUI* pListElement, bool bVisible, DuiLib::NodeData* pNodeData);
		void SetFileState(CListContainerElementUI* pListElement, bool bVisible, DuiLib::NodeData* pNodeData);
		void SetAgreeDenyBtnState(CListContainerElementUI* pListElement, bool bVisible);
		void SetQianyueState(CListContainerElementUI* pListItem, bool bVisible, tstring& strState);

		void UpdateQianyuePackState(tstring& strPackID, tstring& newState);

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

		//是否具有 “执行” 的duty
		bool m_bExcute;

		CDialogBuilder m_dlgBuilder;

		MapPackID2Packs m_mapPackFiles;
	};

//} // DuiLib
