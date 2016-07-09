#pragma once

#include "StdAfx.h"
#include "UIFriendListBase.h"
#define STR_ORG_MEM_LIST_ITEM "ORG_MEM_ListItem_"



class CUIOrgMembersList : public DuiLib::CFriendListBaseUI 
{
public:
	CUIOrgMembersList(CPaintManagerUI *paint_manager);
	//����ͷ��
	void UpdataAvatar(tstring& strImageFile, int index);
	//�û�������memo�������ǳ�
	void UpdateShowName(tstring& strShowName, int index);
	
	Node* AddNode(const FriendListItemInfo& item, Node* parent);

	//���ݱ�ɾ���˺ŵ�nItem ID����ɾ��
	void DeSelectItem(/*tstring& strAccount*/int nItemID);

	//ɾ���˺�ΪstrAccount��List item
	void RemoveItem(int nItemID);

	void Sort();

	void DoEvent(TEventUI& event) ;

public:
	static int CALLBACK CompareListItem(UINT_PTR lPara1, UINT_PTR lPara2, UINT_PTR dwSortType);

};


