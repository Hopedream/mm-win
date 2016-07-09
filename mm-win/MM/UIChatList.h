#pragma once
#include "StdAfx.h"
#include "..\Common\MMDefine.h"
#include "UIDefine.h"


namespace DuiLib
{
	struct ChatInfo
	{
		CDuiString m_strLogo;				//发送消息者的头像
		CDuiString m_strAccount;			//这条消息是谁发的
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
		//设置自己的账户信息
		void SetMySelfAccount(tstring& strAccount);
		//缓存聊天记录
		void CacheChatMessage(CDuiString& strDestAccount, ChatInfo* pchatInfo);
		//重新加载聊天记录
		void ReloadChatList(CDuiString& strAccount);
		vector<fileInfo> getDownLoadFiles(int);

		void ClearMessage(const CDuiString& account);
		//bool AddOfferItem();    //询报价
		//bool AddConsultationItem();   //技术咨询
	private:
		//CControlUI *createOfferControl();
		//CControlUI *createConsultationControl();
		void createFileList(CListUI *, vector<fileInfo>&);
		void EndListDown(int nOffset);
	private:
		CPaintManagerUI* m_pPaintManager;

		//CDialogBuilder m_DlgBuilder;
		//CListContainerElementUI* pControl, CTreeNodeUI* pParent=NULL

		__int64 m_curTime;            //当前时间
		vector<fileInfo> m_lstFiles;         //发送的文件信息列表
		typedef vector<fileInfo> m_lstGetFiles;          //收到的文件列表
		typedef map<int, m_lstGetFiles> MapGetFiles;
		MapGetFiles m_mapGetFiles;
		int m_iTag;                   //文件接受控件标志，便于区分下载哪个文件包
	private:
		tstring m_strMyAcount;

		//缓存我和对方的聊天记录。
		typedef std::list<ChatInfo> ListMessages;
		typedef std::map<CDuiString, ListMessages> MapAccout2Messages;
		MapAccout2Messages m_mapDest2Msgs;
	};
}