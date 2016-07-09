#pragma once
#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"

namespace DuiLib
{
	//聊天记录列表
	struct ContactListItemInfo
	{
		int		   nItemID;
		CDuiString m_strAccount;	//用户账号，如果么有备注就显示账号
		CDuiString m_strID;			//用户ID。
		CDuiString m_strNickName;	//用户昵称
		CDuiString m_strOrgName;	//机构名称
		CDuiString m_strOrgID;		//机构ID
		CDuiString m_strRoomID;		//对于群组类型，通过RoomID来表示唯一性
		CDuiString m_strLogo;		//在线显示亮图，否则灰图，或者离线
		CDuiString m_strLastMsg;	//最后一条聊天记录，没有就空白	
		CDuiString m_strLastTm;		//最后一条聊天时间，没有就空白
		CDuiString m_strMsgCount;	//显示消息个数
		EUsrState m_State;			//当前的状态
		ContactListItemInfo()
		{
			nItemID = -1;
			m_State = enmState_Offline;
		}
	};
	
	class CContactsUI :public CListUI
	{
	public:
		CContactsUI(CPaintManagerUI *paint_manager);
		~CContactsUI();
		bool Add(CControlUI *pControl);
		bool AddAt(CControlUI *pControl, int iIndex);
		bool Remove(CControlUI *pControl);
		bool RemoveAt(int iIndex);
		void RemoveAll();
		void DoEvent(TEventUI& event);
		void AddItem(ContactListItemInfo *item, int indx = -1);

		void AddWelcomeItem(ContactListItemInfo *item, int indx = -1);
		
		//更新显示名称
		void UpdateShowName(tstring& strShowName, int nIndex);

		//更新头像
		void UpdateHeadImage(const tstring& strImageFile, int index);

		//更新最后一次聊天消息
		void SetLastMessage(tstring& strMsg, tstring& strTime, int index);

		//设置联系人未读信息气泡
		void setMsgNum(CDuiString &strNum, int iIndex);
		
		//查询nItemIndex索引位置的Item 是否有聊天气泡，这个只是聊天记录的List才会需要。
		bool CheckIfBubbleExist(int nItemIndex);
		
		//删除账号为strAccount的List item
		void RemoveItem(int nItemID);
		
		//strAccount是否在列表中。
		int GetIndex(int nItemID);
		//CListUI *m_pUI;
		// 置顶当前项
		void topItem(int nItemID);

	private:
		CPaintManagerUI* m_pPaintManager;
		CDialogBuilder m_DlgBuilder;
		//CDuiRect       m_textPadding;
	};
}