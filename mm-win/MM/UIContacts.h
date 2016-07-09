#pragma once
#include "StdAfx.h"
#include "..\Common\UserInfoDef.h"

namespace DuiLib
{
	//�����¼�б�
	struct ContactListItemInfo
	{
		int		   nItemID;
		CDuiString m_strAccount;	//�û��˺ţ����ô�б�ע����ʾ�˺�
		CDuiString m_strID;			//�û�ID��
		CDuiString m_strNickName;	//�û��ǳ�
		CDuiString m_strOrgName;	//��������
		CDuiString m_strOrgID;		//����ID
		CDuiString m_strRoomID;		//����Ⱥ�����ͣ�ͨ��RoomID����ʾΨһ��
		CDuiString m_strLogo;		//������ʾ��ͼ�������ͼ����������
		CDuiString m_strLastMsg;	//���һ�������¼��û�оͿհ�	
		CDuiString m_strLastTm;		//���һ������ʱ�䣬û�оͿհ�
		CDuiString m_strMsgCount;	//��ʾ��Ϣ����
		EUsrState m_State;			//��ǰ��״̬
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
		
		//������ʾ����
		void UpdateShowName(tstring& strShowName, int nIndex);

		//����ͷ��
		void UpdateHeadImage(const tstring& strImageFile, int index);

		//�������һ��������Ϣ
		void SetLastMessage(tstring& strMsg, tstring& strTime, int index);

		//������ϵ��δ����Ϣ����
		void setMsgNum(CDuiString &strNum, int iIndex);
		
		//��ѯnItemIndex����λ�õ�Item �Ƿ����������ݣ����ֻ�������¼��List�Ż���Ҫ��
		bool CheckIfBubbleExist(int nItemIndex);
		
		//ɾ���˺�ΪstrAccount��List item
		void RemoveItem(int nItemID);
		
		//strAccount�Ƿ����б��С�
		int GetIndex(int nItemID);
		//CListUI *m_pUI;
		// �ö���ǰ��
		void topItem(int nItemID);

	private:
		CPaintManagerUI* m_pPaintManager;
		CDialogBuilder m_DlgBuilder;
		//CDuiRect       m_textPadding;
	};
}