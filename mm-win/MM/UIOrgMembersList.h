#pragma once

#include "StdAfx.h"
#include "UIFriendListBase.h"
#define STR_ORG_MEM_LIST_ITEM "ORG_MEM_ListItem_"



class CUIOrgMembersList : public DuiLib::CFriendListBaseUI 
{
public:
	CUIOrgMembersList(CPaintManagerUI *paint_manager);
	//更新头像
	void UpdataAvatar(tstring& strImageFile, int index);
	//用户更新了memo或者是昵称
	void UpdateShowName(tstring& strShowName, int index);
	
	Node* AddNode(const FriendListItemInfo& item, Node* parent);

	//根据被删除账号的nItem ID来来删除
	void DeSelectItem(/*tstring& strAccount*/int nItemID);

	//删除账号为strAccount的List item
	void RemoveItem(int nItemID);

	void Sort();

	void DoEvent(TEventUI& event) ;

public:
	static int CALLBACK CompareListItem(UINT_PTR lPara1, UINT_PTR lPara2, UINT_PTR dwSortType);

};


