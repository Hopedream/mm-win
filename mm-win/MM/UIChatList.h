#pragma once
#include "StdAfx.h"
#include "..\Common\MMDefine.h"
#include "UIDefine.h"


namespace DuiLib
{
	struct ChatInfo
	{
		CDuiString m_strLogo;				//������Ϣ�ߵ�ͷ��
		CDuiString m_strAccount;			//������Ϣ��˭����
		std::vector<CDuiString> m_lstMsg;
		CDuiString m_strRawMsg;
		//CDuiString m_strTime;
		__int64 uTime;
		std::vector<fileInfo> m_lstFiles;
	};

	class CChatListUI : public CListUI
	{
	public:
		CChatListUI(CPaintManagerUI *paint_manager);
		~CChatListUI();
		bool AddMyMsgItem(ChatInfo* pchatInfo);
		bool AddOppositeMsgItem(ChatInfo *pchatInfo);
		bool AddTimeItem(CDuiString& strTm, int extra_height_need = 0);
		bool AddMyFileMsgItem(ChatInfo* pFilesInfo);
		bool AddOppositeFileMsgItem(ChatInfo *pFilesInfo);
		bool AddMyGroupFileItem(tstring& strPackID, tstring& strAvatar, time_t time, sPackItem& onepack);
		bool AddOppositeGroupFileItem(tstring& strPackID, tstring& strAvatar, time_t time, sPackItem& onepack);
		bool AddSysNotification(tstring strAccount);
		bool AddSysItem(CDuiString& strSys);
		bool AddOppositeVoiceItem(ChatInfo *pFilesInof);
		//�����Լ����˻���Ϣ
		void SetMySelfAccount(tstring& strAccount);
		//���������¼
		void CacheChatMessage(CDuiString& strDestAccount, ChatInfo* pchatInfo);
		//���¼��������¼
		void ReloadChatList(CDuiString& strAccount);
		vector<fileInfo> getDownLoadFiles(int);

		void ClearMessage(const CDuiString& account);
		//bool AddOfferItem();    //ѯ����
		//bool AddConsultationItem();   //������ѯ
	private:
		//CControlUI *createOfferControl();
		//CControlUI *createConsultationControl();
		void createFileList(CListUI *, vector<fileInfo>&);
		void EndListDown(int nOffset);
	private:
		CPaintManagerUI* m_pPaintManager;

		//CDialogBuilder m_DlgBuilder;
		//CListContainerElementUI* pControl, CTreeNodeUI* pParent=NULL

		__int64 m_curTime;            //��ǰʱ��
		vector<fileInfo> m_lstFiles;         //���͵��ļ���Ϣ�б�
		typedef vector<fileInfo> m_lstGetFiles;          //�յ����ļ��б�
		typedef map<int, m_lstGetFiles> MapGetFiles;
		MapGetFiles m_mapGetFiles;
		int m_iTag;                   //�ļ����ܿؼ���־���������������ĸ��ļ���
	private:
		tstring m_strMyAcount;

		//�����ҺͶԷ��������¼��
		typedef std::list<ChatInfo> ListMessages;
		typedef std::map<CDuiString, ListMessages> MapAccout2Messages;
		MapAccout2Messages m_mapDest2Msgs;
	};
}