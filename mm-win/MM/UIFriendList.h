#pragma once

#include "StdAfx.h"
#include "UIFriendListBase.h"
#define STR_FRIENDLIST_ITEM "FriendListItem_"



class CUIFriendList : public DuiLib::CFriendListBaseUI 
{
public:
	CUIFriendList(CPaintManagerUI *paint_manager);
	//更新头像
	void UpdataAvatar(tstring& strImageFile, int index);
	//用户更新了memo或者是昵称
	void UpdateShowName(tstring& strShowName, int index);

	//机构修改了用户的权限
	void UpdateDuty(const vector<tstring>& duty, int index);

	
	Node* AddNode(const FriendListItemInfo& item, Node* parent);

	//删除账号为strAccount的List item
	void RemoveItem(int nItemID);

	void ModifyOrgName(Node* pNode, const tstring strOrgName);

	//void Sort();
//
//public:
//	static int CALLBACK CompareListItem(UINT_PTR lPara1, UINT_PTR lPara2, UINT_PTR dwSortType);

};


