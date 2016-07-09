#pragma once

#include "StdAfx.h"
#include "UIFriendListBase.h"
#define STR_FRIENDLIST_ITEM "FriendListItem_"



class CUIFriendList : public DuiLib::CFriendListBaseUI 
{
public:
	CUIFriendList(CPaintManagerUI *paint_manager);
	//����ͷ��
	void UpdataAvatar(tstring& strImageFile, int index);
	//�û�������memo�������ǳ�
	void UpdateShowName(tstring& strShowName, int index);

	//�����޸����û���Ȩ��
	void UpdateDuty(const vector<tstring>& duty, int index);

	
	Node* AddNode(const FriendListItemInfo& item, Node* parent);

	//ɾ���˺�ΪstrAccount��List item
	void RemoveItem(int nItemID);

	void ModifyOrgName(Node* pNode, const tstring strOrgName);

	//void Sort();
//
//public:
//	static int CALLBACK CompareListItem(UINT_PTR lPara1, UINT_PTR lPara2, UINT_PTR dwSortType);

};


