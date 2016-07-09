#include "GroupChatPage.h"
#include "CDialogBuilderCallbackEx.h"
#include "..\include\Global_Def.h"
#include "..\include\MsgWndDefine.h"
#include "..\include\DBTaskModule.h"
#include "FileHelper.h"
#include "UIDefine.h"
#include "mainDlg.h"
#include "UIChatList.h"
#include "UIMenu.h"
#include "Controller.h"
#include "MsgSessionMgr.h"
#include "MUCRoomMgr.h"
#include "GroupChatLogic.h"
#include "TileLayoutResPacket.h"
#include "GroupPackFileDownloadWnd.h"
#include "Utility.h"
#include <ShellAPI.h>
#include "UserManager.h"
#include "webClient.h"
#include "properties_resource.h"
#include "HttpClient.h"
#include "MsgBox.h"
#include "oss_interface.h"
#include "..\json\json.h"
#include "..\Common\regexHelper.h"
#include "..\Common\Logger.h"
#include "StringHelper.h"
#include <sstream>

#include "mainDlg.h"

struct sTmpInfo
{
	fileInfo* pFile;
	void* pThis;
	IOssEventBase* pEventHandle;
	tstring strProjectID;
	tstring strPackID;
	sTmpInfo()
	{
		pFile=NULL;
		pThis=NULL;
		pEventHandle=NULL;
	}
};



CGroupChatPage::CGroupChatPage(CPaintManagerUI* ppm/*=NULL*/)
{
	m_pFilePacketList = NULL;
	m_pListWithProperty = NULL;
	m_pPaintManager = ppm;

	Initialize();

	m_pGroupChatLogic = new CGroupChatLogic;
	m_pGroupChatLogic->AddHandler(this);
	g_pMsgWnd->AddEventHandler(this);

	::InitializeCriticalSection(&m_csUploadFile);
	::InitializeCriticalSection(&m_csFLSection);

	m_bSendFileMode = false;
	//m_pGroupPackDownLoadWnd = NULL;

	m_eSendMode = (eSendMsgMode)GetDBTaskModule()->GetSendMsgHotKey();
}

CGroupChatPage::~CGroupChatPage()
{
	m_pPaintManager->RemoveNotifier(this);

	if (NULL != m_pGroupChatLogic)
	{
		delete m_pGroupChatLogic;
		m_pGroupChatLogic = NULL;
	}

	::DeleteCriticalSection(&m_csUploadFile);
	::DeleteCriticalSection(&m_csFLSection);
}

void CGroupChatPage::Notify(TNotifyUI& msg)
{
	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		tstring strSender = msg.pSender->GetName();

		//切换编辑资料和资料列表的页面
		if (_tcsicmp(msg.pSender->GetName(), "buttonFile_groupchat") == 0)
		{
			OnGotoEditFilePage(CONST_GROUPCHAT_FILE_EDIT_PAGE);
		}
		else if(_tcsicmp(msg.pSender->GetName(), "contract_groupchat") == 0)
		{
			OnGotoEditFilePage(CONST_GROUPCHAT_CONTRACT_PAGE);
		}
		//发送消息
		else if (_tcsicmp(msg.pSender->GetName(), "buttonSend_groupchat") == 0)
		{
			OnSendMessage();
		}
		//选择“Enter”、“Ctrl+Enter”发送
		else if (_tcsicmp(msg.pSender->GetName(), "btnChooseSendtype_groupchat") == 0)
		{
			POINT pt = {msg.ptMouse.x, msg.ptMouse.y};
			CDuiMenu *pMenu = new CDuiMenu(_T("sendTypeDropUISkin.xml"), eUIMenuSendMsgKeyDrop);
			pMenu->Init(mainDlg::getInstance()->GetHWND(), pt);
			pMenu->ShowWindow(TRUE);
		}
		//编辑资料--”文件“
		else if (_tcsicmp(msg.pSender->GetName(), "gm_btn_new_proj_files") == 0)
		{
			OnSelectSendingFile(CONST_GROUPCHAT_FILE_EDIT_PAGE);
		}
		//发送合约--"文件按钮"
		else if(_tcsicmp(msg.pSender->GetName(),BTN_CONTRACT_PAGE_FILE) == 0 || _tcsicmp(msg.pSender->GetName(),BTN_CONTRACT_PAGE_DATA)== 0)
		{
			OnSelectSendingFile(CONST_GROUPCHAT_CONTRACT_PAGE);
		}
		//编辑资料--”保存“
		else if (_tcsicmp(msg.pSender->GetName(), "gm_Buttongroupchat_save") == 0)
		{
			if (CheckIfDataChanged())
			{
				OnBnClickSaveProject();
			}
		}
		//编辑资料--”发送“
		else if (_tcsicmp(msg.pSender->GetName(), "gm_Buttongroupchat_send") == 0)
		{
			OnSendResPacket();
		}

		//收款方与付款方
		else if (_tcsicmp(msg.pSender->GetName(), "OptionIn") == 0)
		{
			OnGetGroupContactProperty();
		}
		else if (_tcsicmp(msg.pSender->GetName(), "OptionIn") == 0)
		{
			OnGetGroupContactProperty();
		}
		//编辑合约 -- “发送合约”
		else if(_tcsicmp(msg.pSender->GetName(),BTN_CONTRACT_PAGE_SEND) == 0)
		{
			OnSendContract();
			OnSendContactMsgToCss();
			m_bSendFileMode = true;
		}
		else if (_tcsicmp(msg.pSender->GetName(), "btn_delete_askfileitem") == 0)
		{
			OnBnClickRemoveOneFile(msg.pSender);
		}
		//”同意“签约包
		else if (_tcsicmp(msg.pSender->GetName(), "gm_button_agree") == 0)
		{
			if(UpdatePackStatus(msg.pSender, tstring("3")))
			{
				CListContainerElementUI* pElement = static_cast<CListContainerElementUI*>(msg.pSender->GetParent()->GetParent()->GetParent());
				if (NULL != pElement)
				{
					Node* pNode = (Node*)pElement->GetTag();
					tstring strPackID = pNode->data().strPackID;
					tstring strRoomID;
					mainDlg::getInstance()->GetCurrGroupRoomID(strRoomID);

					//告知群里面其他成员，更新状态
					//0:默认状态 1:收藏 2:收发 3：同意 4：拒绝
					//#COMMAND#type=groupcontract,0 1 2 3 4,packid,roomname,from,packname,attribute
					ostringstream os;
					CFileHelper helper;
					os<<"#COMMAND#"<<"type="<<helper.GetDescription(enmCmd_GroupQianyuePack).c_str()<<","
						<<"3"<<","
						<<strPackID.c_str()<<","
						<<strRoomID.c_str()<<","
						<<CFileHelper::GetUserName(CController::Instance()->GetXmppClient()->jid().bare())<<","
						<<pNode->data().strPackName<<","
						<<GetAttibuteValue(pNode->data().strPackProp)<<",";

					CController::Instance()->GetRoomManager()->SendMessage(strRoomID, os.str());
				}
			}
		}
		//”拒绝“签约包
		else if (_tcsicmp(msg.pSender->GetName(), "gm_button_deny") == 0)
		{
			if(UpdatePackStatus(msg.pSender, tstring("4")))
			{
				CListContainerElementUI* pElement = static_cast<CListContainerElementUI*>(msg.pSender->GetParent()->GetParent()->GetParent());
				if (NULL != pElement)
				{
					Node* pNode = (Node*)pElement->GetTag();
					tstring strPackID = pNode->data().strPackID;
					tstring strRoomID;
					mainDlg::getInstance()->GetCurrGroupRoomID(strRoomID);
				
					//告知群里面其他成员，更新状态
// 					ostringstream os;
// 					CFileHelper helper;
// 					os<<"#COMMAND#"<<"type="<<helper.GetDescription(enmCmd_GroupQianyuePack).c_str()
// 						<<","<<strPackID.c_str()<<","<<strRoomID.c_str()<<","<<"deny";

					ostringstream os;
					CFileHelper helper;
					os<<"#COMMAND#"<<"type="<<helper.GetDescription(enmCmd_GroupQianyuePack).c_str()<<","
						<<"4"<<","
						<<strPackID.c_str()<<","
						<<strRoomID.c_str()<<","
						<<CFileHelper::GetUserName(CController::Instance()->GetXmppClient()->jid().bare())<<","
						<<pNode->data().strPackName<<","
						<<GetAttibuteValue(pNode->data().strPackProp)<<",";

					CController::Instance()->GetRoomManager()->SendMessage(strRoomID, os.str());
				}
			}
		}
		else if (_tcsicmp(msg.pSender->GetName(), "gm_button_openfile") == 0)
		{
			CListContainerElementUI* pElement = static_cast<CListContainerElementUI*>(msg.pSender->GetParent()->GetParent()->GetParent());
			if (NULL != pElement)
			{
				Node* pNode = (Node*)pElement->GetTag();
				tstring strAliUrl = pNode->data().strAliUrl;
				tstring strFileName = pNode->data().strFileName;
				DWORD dwFileSize = _tcstoul(pNode->data().strFileSizeAccurate.c_str(), NULL, 10);

				fileInfo* pOneFile = new fileInfo;
				pOneFile->strFileUrl = strAliUrl;
				pOneFile->strFilePath = CFileHelper::GetPackDir(pNode->data().strPackName, pNode->data().strPackID);
				pOneFile->strFilePath += strFileName;//用户目录下的临时文件夹。
				pOneFile->dwFileSize = dwFileSize;

				//先判断有没有这个文件了，如有就不下载了。直接打开
				bool bExist = PathFileExists(pOneFile->strFilePath.c_str());
				if (bExist)
				{
					if (getFileSize(pOneFile->strFilePath) == dwFileSize)
					{
						ShellExecute(NULL, NULL, pOneFile->strFilePath.c_str(), NULL,NULL,SW_SHOWNORMAL);
						delete pOneFile;
						return;
					}
				}

				downloadFiles(*pOneFile, NULL);
			}
		}
		//用户点击了“收藏”
		else if (_tcsicmp(msg.pSender->GetName(), "btn_shoucang_packfiles") == 0)
		{
			sPackItem* pOnePack = (sPackItem*)msg.pSender->GetTag();
			if (pOnePack == NULL)
			{
				return;
			}

			tstring strPackID = pOnePack->strPackID;
			if (strPackID.empty())
			{
				return;
			}

			//1:修改状态为：“2”，收发资料包
			CHttpClient reqClient;
			tstring strResponse;
			tstring strToken = CController::Instance()->GetToken();

			//更改pack状态
			tstring strUrlUpdatePack(WEB_SERVER_BASE_URL);
			strUrlUpdatePack += "file/update-pack-status?pack_id=";
			strUrlUpdatePack += strPackID;
			strUrlUpdatePack += "&status=";
			strUrlUpdatePack += "2";

			tstring strOutUTF8Projname;
			CChineseCode::GB2312ToUTF_8(strOutUTF8Projname, strUrlUpdatePack.c_str() , strUrlUpdatePack.length());
			int nSucc = reqClient.Get(strOutUTF8Projname, strToken, strResponse);
			if ( strResponse.find("result") != -1 && strResponse.find("1") != -1)
			{
				CMsgBox* pMsgBox = new CMsgBox("提示","收藏资料包成功！");
				pMsgBox->ShowModal();
			}
			else
			{
				tstring strError;
				if (strResponse.find("6005") != -1)
				{
					strError = "签约资料已被处理";
				}
				else if (strResponse.find("6002") != -1)
				{
					strError ="资料包不存在";
				}
				else if (strResponse.find("6004") != -1)
				{
					strError="创建方无权处理签约资料包";
				}
				else if (strResponse.find("3002") != -1)
				{
					strError = "所需参数无效";
				}
				else if(strResponse.find("3004") != -1)
				{
					strError="不是业务参与方";
				}

				CMsgBox* pMsgBox = new CMsgBox("错误",strError.c_str());
				pMsgBox->ShowModal();	
			}
		}
		//用户点击了“查看”资料包
		else if(_tcsicmp(msg.pSender->GetName(), "btn_open_packfiles") == 0)
		{
			sPackItem* pOnePack = (sPackItem*)msg.pSender->GetTag();
			if (pOnePack == NULL)
			{
				return;
			}

			tstring strPackID = pOnePack->strPackID;
			if (strPackID.empty())
			{
				return;
			}

			//允许多开窗口查看不同的资料包
			map<tstring, CGroupPackFileDownloadWnd*>::iterator it = m_mapPack2DownloadWnd.find(strPackID);
			if (it != m_mapPack2DownloadWnd.end())
			{
				//如果已经打开了，就直接show出来
				it->second->ShowWindow(true);
			}
			else
			{
				CGroupPackFileDownloadWnd* pGroupPackDownLoadWnd = new CGroupPackFileDownloadWnd(m_pPaintManager);
				if (NULL != pGroupPackDownLoadWnd)
				{
					pGroupPackDownLoadWnd->SetMainWndHwnd(m_hMainWnd);
					pGroupPackDownLoadWnd->SetRoomID(m_strLastRoomID);
					pGroupPackDownLoadWnd->CreateWnd();
				}

				pGroupPackDownLoadWnd->Initialize(pOnePack);
				pGroupPackDownLoadWnd->ShowWindow(true);

				m_mapPack2DownloadWnd.insert(make_pair(strPackID, pGroupPackDownLoadWnd));
			}
		}
	}
	else if(_tcsicmp(msg.sType, _T("itemclick")) == 0)
	{
		HandleItemClickEvent(msg);		
	}
}

void CGroupChatPage::Initialize()
{
	//Create group chat page.
	CDialogBuilder DlgBuilder1;
	CDialogBuilderCallbackEx cb1(m_pPaintManager);
	CTabLayoutUI*pLayout = static_cast<CTabLayoutUI*>(m_pPaintManager->FindControl("RecentChatTabLayout"));
	if (NULL == pLayout)
	{
		return;
	}

	CHorizontalLayoutUI* pGroupChatPage = NULL;
	if (!DlgBuilder1.GetMarkup()->IsValid())
	{
		pGroupChatPage = static_cast<CHorizontalLayoutUI*>(DlgBuilder1.Create(_T("tab_group_chat_page.xml"),
			(UINT)0, &cb1, m_pPaintManager));
	}
	else
	{
		pGroupChatPage = static_cast<CHorizontalLayoutUI*>(DlgBuilder1.Create((UINT)0, m_pPaintManager));
	}

	//作为RecentPage里面的第4个页签
	if (NULL != pGroupChatPage)
	{
		pLayout->AddAt(pGroupChatPage, CONST_RECENT_TAB_GROUPCHAT_PAGE);
	}
	
	//初始化自己右边两个子page。
	CTabLayoutUI*pGroupChildLayout = static_cast<CTabLayoutUI*>(m_pPaintManager->FindControl("GroupChatFileLayout"));
	if (NULL == pGroupChildLayout)
	{
		return;
	}
	{
		//加载资料编辑页面
		CDialogBuilder DlgBuilder2;
		CDialogBuilderCallbackEx cb2(m_pPaintManager);
		CVerticalLayoutUI* pGroupEditResPage = NULL;
		if (!DlgBuilder2.GetMarkup()->IsValid())
		{
			pGroupEditResPage = static_cast<CVerticalLayoutUI*>(DlgBuilder2.Create(_T("group_chat_eidtfile_page.xml"),
				(UINT)0, &cb2, m_pPaintManager));
		}
		else
		{
			pGroupEditResPage = static_cast<CVerticalLayoutUI*>(DlgBuilder1.Create((UINT)0, m_pPaintManager));
		}

		//作为GroupPage里面的第1个页签
		if (NULL != pGroupEditResPage)
		{
			pGroupChildLayout->AddAt(pGroupEditResPage, CONST_GROUPCHAT_FILE_EDIT_PAGE);
		}
	}
	{
		//加载资料包列表页面
		CDialogBuilder DlgBuilder3;
		CDialogBuilderCallbackEx cb3(m_pPaintManager);
		CVerticalLayoutUI* pGroupFileListPage = NULL;
		if (!DlgBuilder3.GetMarkup()->IsValid())
		{
			pGroupFileListPage = static_cast<CVerticalLayoutUI*>(DlgBuilder3.Create(_T("group_chat_filelist_page.xml"),
				(UINT)0, &cb3, m_pPaintManager));
		}
		else
		{
			pGroupFileListPage = static_cast<CVerticalLayoutUI*>(DlgBuilder3.Create((UINT)0, m_pPaintManager));
		}

		//作为GroupPage里面的第2个page
		if (NULL != pGroupFileListPage)
		{
			pGroupChildLayout->AddAt(pGroupFileListPage, CONST_GROUPCHAT_FILE_LIST_PAGE);
			m_pFilePacketList = static_cast<CListResPacket*>(m_pPaintManager->FindSubControlByName(pGroupFileListPage, "groupchatpacketfiles"));
		}
	}
	{
		//加载合约页面
		CDialogBuilder DlgBuilder;
		CDialogBuilderCallbackEx cb(m_pPaintManager);
		CVerticalLayoutUI* pGroupContractPage = NULL;
		if (!DlgBuilder.GetMarkup()->IsValid())
		{			
			pGroupContractPage = static_cast<CVerticalLayoutUI*>(DlgBuilder.Create(XML_CONTRACT_FILE_NAME,
				(UINT)0, &cb, m_pPaintManager));

			//加载合约时要加载长协
//			OnGetGroupContactProperty();
		}
		else
		{
			pGroupContractPage = static_cast<CVerticalLayoutUI*>(DlgBuilder.Create((UINT)0, m_pPaintManager));
		}

		//作为GroupPage里面的第2个page
		if (NULL != pGroupContractPage)
		{
			pGroupChildLayout->AddAt(pGroupContractPage, CONST_GROUPCHAT_CONTRACT_PAGE);
		}
	}

	if (pGroupChildLayout->GetCount()>0)
	{
		pGroupChildLayout->SelectItem(CONST_GROUPCHAT_FILE_EDIT_PAGE);
	}


	//初始化编辑页面的ComboBox
	InitComboItems();

	m_pListWithProperty = static_cast<CTileLayoutResPacket*>( m_pPaintManager->FindControl("gm_tileListRespackets"));
	m_pContractFiles = static_cast<CTileLayoutResPacket*>(m_pPaintManager->FindControl(PACKET_CONTRACT_PAGE_FILES));
	//监听全局消息接口
	m_pPaintManager->AddNotifier(this);

}

void CGroupChatPage::RefreshGroupChatList(tstring& strRoomID)
{
	if (m_pGroupChatLogic != NULL)
	{
		tstring strMyAccount = CController::Instance()->GetXmppClient()->jid().bare();
		CChatListUI *pChatlst = static_cast<CChatListUI*>(m_pPaintManager->FindControl("listgroupchat"));
		pChatlst->RemoveAll();

		ListGroupChat ChatList;
		if(m_pGroupChatLogic->GetCachedChatList(strRoomID, ChatList))
		{
			for(ListGroupChat::iterator it = ChatList.begin(); it != ChatList.end(); ++it)
			{
				sGroupChatItem& oneMsg = *it;
				
			
				tstring strStatus;
				tstring strSenderAccount = oneMsg.strSenderNick+"@test-d";
				
				sPackItem onePack;
				tstring sPackName;
				if(CheckIfPackRequest(oneMsg.strTextContent, onePack))
				{
					//插入文件Item,需要区分对方和本方，并check是否本地有缓存，没有缓存就去云端拉取
					ChatInfo chatinfo;
					chatinfo.m_lstMsg.push_back(oneMsg.strTextContent.c_str());
					chatinfo.m_strAccount = oneMsg.strSenderAccount.c_str();
					chatinfo.m_strRawMsg = oneMsg.strTextContent.c_str();
					chatinfo.uTime = oneMsg.tSendTime;;

					tstring strAccount = oneMsg.strSenderNick+"@test-d";
					tstring strAvatar = CFileHelper::GetHigtLightAvatar(strAccount);
					
					tstring strSenderAccount = oneMsg.strSenderNick+"@test-d";
					if(_tcsicmp(strSenderAccount.c_str(), strMyAccount.c_str()) == 0)
					{
						pChatlst->AddMyGroupFileItem(onePack.strPackID, strAvatar, oneMsg.tSendTime,onePack); 
					}
					else
					{
						pChatlst->AddOppositeGroupFileItem(onePack.strPackID, strAvatar, oneMsg.tSendTime,onePack);
					}
				}
				else if(CheckIfGroupQianyuePack(oneMsg.strTextContent, onePack.strPackID, strRoomID, strStatus))
				{
					//更新cache
					UpdatePackCache(strRoomID, onePack.strPackID, strStatus);

					//更新Ui
					m_pFilePacketList->UpdateQianyuePackState(onePack.strPackID, strStatus);
				}
				else if (CheckIfPackDownloadReceipt(oneMsg.strTextContent,sPackName))
				{
					CDuiString sReply;
					sReply.Format("%s%s%s",oneMsg.strSenderNick.c_str(),"下载了资料包",sPackName.c_str());
					pChatlst->AddTimeItem(sReply, 10);//借用下
				}
				else
				{
					ChatInfo chatinfo;
					chatinfo.m_lstMsg.push_back(oneMsg.strTextContent.c_str());
					chatinfo.m_strAccount = oneMsg.strSenderAccount.c_str();
					chatinfo.m_strRawMsg = oneMsg.strTextContent.c_str();
					chatinfo.uTime = oneMsg.tSendTime;;

					tstring strAccount = oneMsg.strSenderNick+"@test-d";
					tstring strAvatar = CFileHelper::GetHigtLightAvatar(strAccount);
					chatinfo.m_strLogo = strAvatar.c_str();	


					if (_tcsicmp(strSenderAccount.c_str(), strMyAccount.c_str()) == 0)
					{
						pChatlst->AddMyMsgItem(&chatinfo);
					}
					else
					{
						pChatlst->AddOppositeMsgItem(&chatinfo);
					}
				}
			}
		}
	}
}

void CGroupChatPage::RefreshGroupPackList(tstring& strRoomID)
{
	//如果是在编辑页面，就不刷新？
	CTabLayoutUI*pGroupChildLayout = static_cast<CTabLayoutUI*>(m_pPaintManager->FindControl("GroupChatFileLayout"));
	if (NULL != pGroupChildLayout)
	{
		if (CONST_GROUPCHAT_FILE_EDIT_PAGE == pGroupChildLayout->GetCurSel())
		{
			return;
		}
	}	


	if(!IsSavingFile())
	{	
		//读取cache
		::EnterCriticalSection(&m_csFLSection);
		map<tstring, MapPackID2Packs>::iterator itFind = m_mapRoom2PackList.find(strRoomID);
		if (itFind != m_mapRoom2PackList.end())
		{
			//清楚界面的List。
			int nCount = m_pFilePacketList->GetCount();
			if (nCount>0)
			{
				m_pFilePacketList->RemoveAll();
			}

			//一个Room下的Packet列表
			MapPackID2Packs& mapPacks = itFind->second;
			for (MapPackID2Packs::iterator it = mapPacks.begin(); 
				it != mapPacks.end(); ++it)
			{
				sPackItem& onePack = it->second;

				//添加一个packItem
				FriendListItemInfoEx item;
				item.folder = true;
				item.strPackIcon = DEF_PACK_ICON;
				item.strPackID = onePack.strPackID;
				//item.strOrgName = onePack.strPackName;
				item.strOrgID = onePack.strOrgID;
				item.strOrgName = onePack.strOrgName;
				item.strPackName = onePack.strPackName;
				item.strPackProp = onePack.strPackProp;
				item.nPackStatus = onePack.nPackStatus;
				item.strPackSender = onePack.strPackSender;
				item.strPackSize = onePack.strPackSize;
				item.strPackTime = onePack.strPackTime;
				item.strPackCreater = onePack.strPackCreaterPhone;
				item.itemId = GetNextItemID();

				m_pFilePacketList->SetExcuteDuty(IsAbleToExecute());
				Node* pParentNode = m_pFilePacketList->AddNode(item, NULL);

				if (onePack.listFile.size()>0)
				{
					for (list<sPackFileItem>::iterator itChild = onePack.listFile.begin();
						itChild != onePack.listFile.end(); ++itChild)
					{
						sPackFileItem& oneFile = *itChild;

						FriendListItemInfoEx itemLeaf;
						itemLeaf.folder = false;
						itemLeaf.itemId = GetNextItemID();
						itemLeaf.strOrgID = onePack.strOrgID;
						itemLeaf.strOrgName = onePack.strOrgName;
						itemLeaf.strPackID = onePack.strPackID;//每个文件都带上资料包的Id和name
						itemLeaf.strPackName = onePack.strPackName;
						itemLeaf.strFileIcon = DEF_AVATAR_NOR;
						itemLeaf.strFileName= oneFile.strFileName;
						itemLeaf.strFileSize = oneFile.strFileSizeShow;
						itemLeaf.strFileSizeAccurate = oneFile.strFileSizeAccurate;
						itemLeaf.strFileType = oneFile.strFileType;
						itemLeaf.strFileUrl = oneFile.strFileUrl;

						m_pFilePacketList->AddNode(itemLeaf,pParentNode);
					}
				}

			}			
		}
		::LeaveCriticalSection(&m_csFLSection);

	}
}

void CGroupChatPage::OnGotoEditFilePage(const int nIndex)
{
	CTabLayoutUI*pGroupChildLayout = static_cast<CTabLayoutUI*>(m_pPaintManager->FindControl("GroupChatFileLayout"));
	if (NULL == pGroupChildLayout)
	{
		return;
	}
	if(nIndex == CONST_GROUPCHAT_FILE_EDIT_PAGE)
	{
		//显示资料编辑页面
		if (pGroupChildLayout->GetCurSel() == CONST_GROUPCHAT_FILE_EDIT_PAGE)
		{
			pGroupChildLayout->SelectItem(CONST_GROUPCHAT_FILE_LIST_PAGE);
		}
		else
		{
			pGroupChildLayout->SelectItem(CONST_GROUPCHAT_FILE_EDIT_PAGE);
			//这个时候需要清空整个编辑页面的内容。
			ClearEditInfo();
		}
	}
	else if(nIndex == CONST_GROUPCHAT_CONTRACT_PAGE)
	{
		//显示发送合约页面
		/*orgInfo info;
		CWebClient::GetMyOrg(info);
		if(_tcsicmp(info.strName.c_str(),"-1")
		{
			CMsgBox* pMsgBox = new CMsgBox("发送合约","您不属于任何机构,无法发送合约",MSGBOX_ICON_WARNING, MSGBOX_IDOK);
			pMsgBox->ShowModal();
			return;
		}*/
		if (pGroupChildLayout->GetCurSel() != CONST_GROUPCHAT_CONTRACT_PAGE)
		{
			pGroupChildLayout->SelectItem(CONST_GROUPCHAT_CONTRACT_PAGE);
			OnGetGroupContactProperty();
		}
	}
}

void CGroupChatPage::ClearEditInfo()
{
	CControlUI* pResPackName = (CControlUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl("gm_edit_res_name"));
	if (NULL != pResPackName)
	{
		pResPackName->SetText(_T(""));
	}

	CControlUI* pControlMemo = (CControlUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl("gm_richedit_create_newproj_memo"));
	if (NULL != pControlMemo)
	{
		pControlMemo->SetText(_T(""));
	}


	CComboUI* pComboProperty = (CComboUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl("gm_combo_res_property"));
	pComboProperty->SelectItem(0);


	m_pListWithProperty->RemoveAll();

}
//聊天框区域--发送
void CGroupChatPage::OnSendMessage()
{
	CRichEditUI *pEditInput = static_cast<CRichEditUI*>(m_pPaintManager->FindControl("inputEdit_groupchat"));
	if (NULL == pEditInput)
	{
		return;
	}

	bool bIsEmptyMsg = CRegexHelper::IsEmptyString(pEditInput->GetText().GetData());
	if (bIsEmptyMsg || pEditInput->GetTextLength() > MAX_SEND_MSG_LEN)
	{
		if (bIsEmptyMsg) pEditInput->SetText(_T(""));

		RECT rect;
		GetClientRect(mainDlg::getInstance()->GetHWND(), &rect);
		POINT pt;
		pt.x = rect.right - 450;
		pt.y = rect.bottom - 80;
		CDuiMenu *pMenu = new CDuiMenu(_T("emptyMsgReminder.xml"), bIsEmptyMsg? eUIMenuMsgEmpty: eUIMenuMsgTooLog);
		pMenu->Init(mainDlg::getInstance()->GetHWND(), pt);
		pMenu->ShowToolTipBox();		
		return;
	}

	CDuiString strValue = pEditInput->GetText();

	//clear input.
	pEditInput->SetText((_T("")));
	
	//send mesage
	if (NULL != CController::Instance()->GetRoomManager())
	{
		//check if empty.
		if (strValue.GetLength() == 0)
		{
			return;
		}
		if (!m_strLastRoomID.empty())
		{
			CController::Instance()->GetRoomManager()->SendMessage(m_strLastRoomID, strValue.GetData());
		}
	}
}

//编辑资料--”文件“被点击
void CGroupChatPage::OnSelectSendingFile(const int nIndex)
{
	vector<tstring> vecSelectedFiles;
	if (getSelectedFiles(vecSelectedFiles))
	{
		int nSize = vecSelectedFiles.size();
		if (nSize>0)
		{
			for (int i=0; i<nSize; i++)
			{
				tstring filePath = vecSelectedFiles[i];
				tstring strValue = filePath.c_str();
				//截取文件名
				//1. 去除文件夹
				int iPos=strValue.find_last_of("\\");
				strValue = strValue.substr(iPos + 1); 
				CDuiString strFileFullName = strValue.c_str();
				//2. 去除文件类型后缀
				iPos = strValue.find_last_of('.');
				CDuiString strfileType = strValue.substr(iPos+1).c_str();
				CDuiString strFileName = strValue.substr(0, iPos).c_str();

				fileInfo oneFile;
				oneFile.strFileFullName = strFileFullName;
				oneFile.strFileName = strFileName;
				oneFile.strFilePath = filePath;
				oneFile.strFileType = strfileType;
				oneFile.dwFileSize = getFileSize(vecSelectedFiles[i]);

				if(oneFile.dwFileSize <= 0)
				{
					CMsgBox* pMsgBox = new CMsgBox("上传文件为空","文件内容不能为空！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
					pMsgBox->ShowModal();
					return;
				}
				//
				int nComboSelectItem = m_pComboResProperty->GetCurSel();
				//无属性资料包List
				//if (nComboSelectItem == 0)
				//{
				//m_pListWithNoProperty->AddOneItem(&oneFile);
				//}
				//else
				//{
				if(nIndex == CONST_GROUPCHAT_CONTRACT_PAGE)
				{
					if(m_pContractFiles->GetCount() > 10)
					{
						CMsgBox* pMsgBox = new CMsgBox("合约文件过多","您已经添加10个文件，无法继续添加文件！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
						pMsgBox->ShowModal();
						return;
					}
					m_pContractFiles->AddOneItem(&oneFile);
				}
				else if(nIndex == CONST_GROUPCHAT_FILE_EDIT_PAGE)
					m_pListWithProperty->AddOneItem(&oneFile);
				//}
			}
		}	
	}
}
//编辑资料-”保存“被点击 --并没有卵用,这是假的
void CGroupChatPage::OnSaveLocalFiles()
{
	//
	CTabLayoutUI*pGroupChildLayout = static_cast<CTabLayoutUI*>(m_pPaintManager->FindControl("GroupChatFileLayout"));
	if (NULL == pGroupChildLayout)
	{
		return;
	}

	//显示资料编辑页面
	pGroupChildLayout->SelectItem(CONST_GROUPCHAT_FILE_LIST_PAGE);
}
//编辑资料--”发送“被点击
void CGroupChatPage::OnSendResPacket()
{
	//保存文件
	if (CheckIfDataChanged())
	{
		OnBnClickSaveProject();

		//当文件上传完毕后，异步的处理部分UI相关。
		m_bSendFileMode = true;
	}
	else
	{
		CMsgBox* pMsgBox = new CMsgBox("提示", "当前资料包信息没有更改，不可以重复保存！", MSGBOX_ICON_WARNING, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}
}

void CGroupChatPage::CacheGroupMessage(tstring& strRoomId)
{
	vector<_tGroupChatMessage> vecGroupMsg;
	if(GetDBTaskModule()->LoadGroupChatMsg(strRoomId, vecGroupMsg))
	{
		int nCount = vecGroupMsg.size();
		if (nCount>0)
		{
			for (int i=0; i<nCount; i++)
			{
				_tGroupChatMessage& oneLocalMsg = vecGroupMsg[i];
				sGroupChatItem* pTmp = new sGroupChatItem;
				pTmp->strRoomID = oneLocalMsg.strRoomID;
				pTmp->strSenderAccount = oneLocalMsg.strSenderAccount;
				pTmp->strSenderNick = oneLocalMsg.strSenderPhone;
				pTmp->strTextContent = oneLocalMsg.strMsg;
				pTmp->tSendTime = oneLocalMsg.m_timeChat;

				//本地缓存
				if (NULL != m_pGroupChatLogic)
				{
					m_pGroupChatLogic->CacheChatData(*pTmp);
				}

				delete pTmp;
				//插入到UI===>不插入了，在刷新的时候，会设置最近联系人列表的第一项，这个时候，会刷新缓存里面的数据到界面上。
				//g_pMsgWnd->PostMessage(WM_GROUPMSG_RECEIVED, (WPARAM)pTmp);
			}

			//设置最后一条消息记录
			_tGroupChatMessage& oneLocalMsg = vecGroupMsg[nCount-1];
			sGroupChatItem* pTmp = new sGroupChatItem;
			pTmp->strRoomID = oneLocalMsg.strRoomID;
			pTmp->strSenderAccount = oneLocalMsg.strSenderAccount;
			pTmp->strSenderNick = oneLocalMsg.strSenderPhone;
			pTmp->strTextContent = oneLocalMsg.strMsg;
			pTmp->tSendTime = oneLocalMsg.m_timeChat;
			::SendMessage(m_hMainWnd, WM_UPDATE_LASTGROUPMSG, (WPARAM)pTmp, (LPARAM)1);
			delete pTmp;
		}
		
	}
}

void CGroupChatPage::InitGroupMsg(tstring& strRoomId)
{
	vector<_tGroupChatMessage> vecGroupMsg;
	if(GetDBTaskModule()->LoadGroupChatMsg(strRoomId, vecGroupMsg))
	{
		int nCount = vecGroupMsg.size();
		for (int i=0; i<nCount; i++)
		{
			_tGroupChatMessage& oneLocalMsg = vecGroupMsg[i];
			sGroupChatItem* pTmp = new sGroupChatItem;
			pTmp->strRoomID = oneLocalMsg.strRoomID;
			pTmp->strSenderAccount = oneLocalMsg.strSenderAccount;
			pTmp->strSenderNick = oneLocalMsg.strSenderPhone;
			pTmp->strTextContent = oneLocalMsg.strMsg;
			pTmp->tSendTime = oneLocalMsg.m_timeChat;

			//本地缓存
			if (NULL != m_pGroupChatLogic)
			{
				m_pGroupChatLogic->CacheChatData(*pTmp);
			}

			//插入到UI
			g_pMsgWnd->SendMessage/*PostMessage*/(WM_GROUPMSG_RECEIVED, (WPARAM)pTmp);
		}
	}
}

void CGroupChatPage::OnReceveGroupMsg(sGroupChatItem& newMsg)
{
	sGroupChatItem* pTmp = new sGroupChatItem;
	pTmp->strPackID = newMsg.strPackID;
	pTmp->strRoomID = newMsg.strRoomID;
	pTmp->strSenderAccount = newMsg.strSenderAccount;
	pTmp->strSenderNick = newMsg.strSenderNick;
	pTmp->strTextContent = newMsg.strTextContent;
	pTmp->tSendTime = newMsg.tSendTime;

	g_pMsgWnd->PostMessage(WM_GROUPMSG_RECEIVED, (WPARAM)(pTmp));
}

void CGroupChatPage::OnMessage(UINT uMsg, WPARAM w, LPARAM l)
{
	if (uMsg == WM_GROUPMSG_RECEIVED)
	{
		sGroupChatItem* pTmp = (sGroupChatItem*)w;
		tstring strMsg = pTmp->strTextContent;
		tstring strSenderAccount = pTmp->strSenderAccount;
		tstring strSenderNick = pTmp->strSenderNick;
		tstring strMyAccount = CController::Instance()->GetXmppClient()->jid().bare();
		tstring strMyNickName = CController::Instance()->GetXmppClient()->jid().username();
		CChatListUI *pChatlst = static_cast<CChatListUI*>(m_pPaintManager->FindControl("listgroupchat"));
		//切换最近联系人到这个room，然后往这个room里面去插入消息，否则会出现消息串入到其他群显示了。。
		DuiLib::ContactListItemInfo& currSelectedItem = mainDlg::getInstance()->getCurItem();
		
		bool bCurRoomSelected = false;
		if (_tcsicmp(currSelectedItem.m_strRoomID.GetData(), pTmp->strRoomID.c_str()) == 0)
		{
			bCurRoomSelected = true;
		}

		if (pTmp != NULL)
		{
			bool bTextmsg = true;
		
			
			tstring strStatus;

			sPackItem onePack;
			tstring sPackName;
			if(CheckIfPackRequest(strMsg, onePack))
			{
				onePack.strPackTime = GetFullTimeString(pTmp->tSendTime);

				//1.更新聊天界面
				if (bCurRoomSelected)
				{
					tstring strSenderAccount = pTmp->strSenderNick+"@test-d";
					tstring strAvatar = CFileHelper::GetHigtLightAvatar(strSenderAccount);
					if(_tcsicmp(strSenderAccount.c_str(), strMyAccount.c_str()) == 0)
					{
						pChatlst->AddMyGroupFileItem(onePack.strPackID, strAvatar, pTmp->tSendTime,onePack);//AddMyFileMsgItem(&chatinfo);  
					}
					else
					{
						pChatlst->AddOppositeGroupFileItem(onePack.strPackID, strAvatar, pTmp->tSendTime,onePack);//AddOppositeFileMsgItem(&chatinfo);
					}
				}
				

				//2.更新FileList，如果cache里面没有，就添加一个。
				if (!IsPackExist(pTmp->strRoomID, onePack.strPackID))
				{
					AddNewPackItem(pTmp->strRoomID, onePack.strPackID);
				}

				//3.刷新最新的PackList
				RefreshGroupPackList(pTmp->strRoomID);

				bTextmsg = false;
			}
			else if(CheckIfGroupQianyuePack(strMsg, onePack.strPackID, pTmp->strRoomID, strStatus))
			{
				//更新缓存：m_mapRoom2PackList.second
				UpdatePackCache(pTmp->strRoomID, onePack.strPackID, strStatus);
				//更新界面
				m_pFilePacketList->UpdateQianyuePackState(onePack.strPackID, strStatus);
				bTextmsg = false;
			}
			else if (CheckIfPackDownloadReceipt(strMsg,sPackName))
			{
				CDuiString sReply;
				sReply.Format("%s%s%s",strSenderNick.c_str(),"下载了资料包",sPackName.c_str());
				pChatlst->AddTimeItem(sReply, 10);//借用下
			}
			//文本消息
			else
			{
				ChatInfo chatinfo;
				chatinfo.m_lstMsg.push_back(pTmp->strTextContent.c_str());
				chatinfo.m_strAccount = pTmp->strSenderAccount.c_str();
				chatinfo.m_strRawMsg = pTmp->strTextContent.c_str();
				chatinfo.uTime = pTmp->tSendTime;

				tstring strAccount = pTmp->strSenderNick+"@test-d";
				tstring strAvatar = CFileHelper::GetHigtLightAvatar(strAccount);
				chatinfo.m_strLogo = strAvatar.c_str();		

				if (bCurRoomSelected)
				{
					if (_tcsicmp(strAccount.c_str(), strMyAccount.c_str()) == 0)
					{
						pChatlst->AddMyMsgItem(&chatinfo);
					}
					else
					{
						pChatlst->AddOppositeMsgItem(&chatinfo);
					}
					SetTimer(m_hMainWnd, WM_TIMER_GROUPCHATPAGEENDDOWN, 50, NULL);
				}
			}

			if (m_hMainWnd != NULL)
			{
				::SendMessage(m_hMainWnd, WM_UPDATE_LASTGROUPMSG, (WPARAM)pTmp, (LPARAM)bTextmsg);
			}

			delete pTmp;
		}
	}
	else if(uMsg == WM_GROUPPACK_UPLOADED)
	{
		//文件上传完毕了，开始在聊天框中发送一个文本消息到群中。
		if (m_bSendFileMode)
		{
			tstring strRoomID = (char*)w;
			tstring strPackID = (char*)l;

			//将资料包状态更新为：-1，默认状态。
			CHttpClient reqClient;
			tstring strResponse;
			tstring strToken = CController::Instance()->GetToken();

			//更改pack状态
			tstring strUrlUpdatePack(WEB_SERVER_BASE_URL);
			strUrlUpdatePack += "file/update-pack-status?pack_id=";
			strUrlUpdatePack += strPackID;
			strUrlUpdatePack += "&status=";
			strUrlUpdatePack += "-1";//"0";

			tstring strOutUTF8Projname;
			CChineseCode::GB2312ToUTF_8(strOutUTF8Projname, strUrlUpdatePack.c_str() , strUrlUpdatePack.length());
			int nSucc = reqClient.Get(strOutUTF8Projname, strToken, strResponse);
			if ( strResponse.find("1") != -1)
			{
				//return true;
			}
			else
			{
				CMsgBox* pMsgBox = new CMsgBox("错误","修改该资料包状态失败！");
				pMsgBox->ShowModal();
				//return false;			
			}



			//2.告知群其他成员文件包的路径,收到这个消息之前，已经InitPackFiles，更新了最新的文件列表的cache
			::EnterCriticalSection(&m_csFLSection);
			map<tstring, MapPackID2Packs>::iterator itFind = m_mapRoom2PackList.find(strRoomID);
			if (itFind != m_mapRoom2PackList.end())
			{
				MapPackID2Packs& mapPackLists = itFind->second;
				MapPackID2Packs::iterator itChildFind = mapPackLists.find(strPackID);
				if (itChildFind != mapPackLists.end())
				{
					sPackItem& onePack = itChildFind->second;
					ostringstream os;
					CFileHelper helper;
					os<<"#COMMAND#"<<"type="<<helper.GetDescription(enmCmd_GroupPackFile).c_str()<<","
						<<"{"
						<<"\"createBy\":\""<<onePack.strPackCreaterPhone<<"\","
						<<"\"packid\":\""<<onePack.strPackID<<"\","
						<<"\"attribute\":\""<<GetAttibuteValue(onePack.strPackProp)<<"\","
						<<"\"packname\":\""<<onePack.strPackName<<"\","
						<<"\"aliNormalFileList\":[";
						
						int nFileCount = onePack.listFile.size();
						int nIdx=0;
						for (list<sPackFileItem>::iterator it = onePack.listFile.begin(); 
							it != onePack.listFile.end(); ++it)
						{
							os<<"{"
							<<"\"file_name\":\""<<it->strFileName<<"\","
							  <<"\"file_type\":\""<<it->strFileType<<"\","
							  <<"\"file_size\":\""<<it->strFileSizeAccurate<<"\","
							  <<"\"ali_file_url\":\""<<it->strFileUrl<<"\""
							<<"}";

							nIdx++;
							if (nIdx <nFileCount)
							{
								os<<",";
							}
						}

						os<<"]}";

					CController::Instance()->GetRoomManager()->SendMessage(strRoomID, os.str());
					ClearContractPage();
					m_bSendFileMode = false;

				}
			}
			::LeaveCriticalSection(&m_csFLSection);			
		}		
	}
}

void CGroupChatPage::SetCurrentRoom(tstring& strRoomID)
{
	//bool bNeedUpdate = false;
	//if (_tcsicmp(m_strLastRoomID.c_str(), strRoomID.c_str()) != 0)
	//{
		m_strLastRoomID = strRoomID;
	//	bNeedUpdate = true;
	//}

	//if (bNeedUpdate)
	//{
		//刷新聊天记录
		RefreshGroupChatList(strRoomID);

		//刷新文件资料包列表
		RefreshGroupPackList(strRoomID);
	//}
}

void CGroupChatPage::InitComboItems()
{
	m_pComboResProperty = static_cast<CComboUI*>(m_pPaintManager->FindControl("gm_combo_res_property"));

	//Init comboBox items
	if (NULL != m_pComboResProperty)
	{
		CListLabelElementUI* pElement0 = new CListLabelElementUI;
		if (NULL != pElement0)
		{
			pElement0->SetText("无属性");
			pElement0->SetName("comboitem_noproperty");
			pElement0->SetTag(0);
			m_pComboResProperty->Add(pElement0);
		}

		CListLabelElementUI* pElement1 = new CListLabelElementUI;
		if (NULL != pElement1)
		{
			pElement1->SetText("询价单");
			pElement1->SetName("comboitem_xjd");
			pElement1->SetTag(1);
			m_pComboResProperty->Add(pElement1);
		}

		CListLabelElementUI* pElement2 = new CListLabelElementUI;
		if (NULL != pElement2)
		{
			pElement2->SetText("报价单");
			pElement2->SetName("comboitem_bjd");
			pElement2->SetTag(2);
			m_pComboResProperty->Add(pElement2);
		}

		CListLabelElementUI* pElement3 = new CListLabelElementUI;
		if (NULL != pElement3)
		{
			pElement3->SetText("合同补充条款");
			pElement3->SetName("comboitem_htbctk");
			pElement3->SetTag(3);
			m_pComboResProperty->Add(pElement3);
		}
		CListLabelElementUI* pElement4 = new CListLabelElementUI;
		if (NULL != pElement4)
		{
			pElement4->SetText("合约条款修改与替换");
			pElement4->SetName("comboitem_hytkxgyth");
			pElement4->SetTag(4);
			m_pComboResProperty->Add(pElement4);
		}
		CListLabelElementUI* pElement5 = new CListLabelElementUI;
		if (NULL != pElement5)
		{
			pElement5->SetText("合约中规定的交付文档");
			pElement5->SetName("comboitem_hyzgddjfwd");
			pElement5->SetTag(5);
			m_pComboResProperty->Add(pElement5);
		}
		CListLabelElementUI* pElement6 = new CListLabelElementUI;
		if (NULL != pElement6)
		{
			pElement6->SetText("发货通知单");
			pElement6->SetName("comboitem_fhtzd");
			pElement6->SetTag(6);
			m_pComboResProperty->Add(pElement6);
		}
		CListLabelElementUI* pElement7 = new CListLabelElementUI;
		if (NULL != pElement7 )
		{
			pElement7->SetText("发运告知单");
			pElement7->SetName("comboitem_fygzd");
			pElement7->SetTag(7);
			m_pComboResProperty->Add(pElement7);
		}
		CListLabelElementUI* pElement8 = new CListLabelElementUI;
		if (NULL != pElement8)
		{
			pElement8->SetText("装箱单");
			pElement8->SetName("comboitem_zxd");
			pElement8->SetTag(8);
			m_pComboResProperty->Add(pElement8);
		}
		CListLabelElementUI* pElement9 = new CListLabelElementUI;
		if (NULL != pElement9)
		{
			pElement9->SetText("收货单");
			pElement9->SetName("comboitem_shd");
			pElement9->SetTag(9);
			m_pComboResProperty->Add(pElement9);
		}
		CListLabelElementUI* pElement10 = new CListLabelElementUI;
		if (NULL != pElement10 )
		{
			pElement10->SetText("验收通知单");
			pElement10->SetName("comboitem_ystzd");
			pElement10->SetTag(10);
			m_pComboResProperty->Add(pElement10);
		}
		CListLabelElementUI* pElement11 = new CListLabelElementUI;
		if (NULL != pElement11)
		{
			pElement11->SetText("付款凭证");
			pElement11->SetName("comboitem_fkpz");
			pElement11->SetTag(11);
			m_pComboResProperty->Add(pElement11);
		}

		m_pComboResProperty->SelectItem(0);
	}
}

void CGroupChatPage::OnBnClickRemoveOneFile(CControlUI* pControl)
{
	CContainerUI* pContainer = (CContainerUI*)(pControl->GetParent());
	if (NULL != pContainer)
	{
		//for test.
		//int nTag = pContainer->GetTag();
		//int nIndex = m_pResFileItem->GetItemIndex(pContainer);
		if (NULL != pContainer->GetParent())
		{
			//清除task信息
			tstring strName = pContainer->GetParent()->GetName();
			fileInfo* pFile = (fileInfo*)(pContainer->GetTag());
			if(ClearTaskInfo(pFile->strFilePath))
			{
			}

			//清除最近数据里面的文件列表
			for (map<tstring, DWORD>::iterator it = m_sLastSavedProjData.mapFile2Size.begin();
				it != m_sLastSavedProjData.mapFile2Size.end(); ++it)
			{
				if (_tcsicmp(it->first.c_str(), pFile->strFileFullName.c_str()) == 0)
				{
					m_sLastSavedProjData.mapFile2Size.erase(it);
					break;
				}
			}

			delete pFile;
			pFile = NULL;

			((CContainerUI*)pContainer->GetParent())->Remove(pContainer);
		}
	}

}

bool CGroupChatPage::ClearTaskInfo(tstring& strFullPath)
{
	bool bSucc = false;

	::EnterCriticalSection(&m_csUploadFile);
	for (map<tstring, sTaskItem>::iterator it = m_mapSaveFileState.begin();
		it != m_mapSaveFileState.end(); ++it)
	{
		if (_tcsicmp(it->second.strPathFile.c_str(), strFullPath.c_str()) == 0)
		{
			if (it->second.pOssObject != NULL)
			{
				//如果还在执行，就终止掉。并清除这个任务信息
				if (it->second.bSaving)
				{
					it->second.pOssObject->StopCurrentTask();
					if(WaitForSingleObject(it->second.hExitEvent, INFINITE) == WAIT_OBJECT_0)
					{
						bSucc = true;
					}
				}

				delete it->second.pOssObject;
				it->second.pOssObject = NULL;
				m_mapSaveFileState.erase(it);

				break;
			}	
		}
	}
	::LeaveCriticalSection(&m_csUploadFile);

	return bSucc;
}

bool CGroupChatPage::CheckIfDataChanged()
{
	bool bChanged = false;

	if (!bChanged)
	{
		CComboUI* pComboProperty = (CComboUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl("gm_combo_res_property"));
		tstring strProperty = pComboProperty->GetText();
		if (_tcsicmp(strProperty.c_str(), m_sLastSavedProjData.strResProperty.c_str()) != 0)
		{
			bChanged = true;
		}
	}

	if (!bChanged)
	{
		CControlUI* pResPackName = (CControlUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl("gm_edit_res_name"));
		tstring strResPackName = pResPackName->GetText();
		if (_tcsicmp(strResPackName.c_str(), m_sLastSavedProjData.strResName.c_str()) != 0)
		{
			bChanged = true;
		}
	}

	if (!bChanged)
	{
		CControlUI* pControlMemo = (CControlUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl("gm_richedit_create_newproj_memo"));
		tstring strMemo = pControlMemo->GetText();
		if (_tcsicmp(strMemo.c_str(), m_sLastSavedProjData.strMemo.c_str()) != 0)
		{
			bChanged = true;
		}
	}

	if (!bChanged)
	{
		//先看文件个数有没有变化。
		int nFileCount = m_pListWithProperty->GetCount();
		if (nFileCount != m_sLastSavedProjData.mapFile2Size.size())
		{
			bChanged = true;
		}

		//比较文件名+大小可变化了。
		if (!bChanged)
		{
			if (nFileCount>0)
			{
				for (int i=0; i<nFileCount; i++)
				{
					CContainerUI* pListItem = (CContainerUI*)m_pListWithProperty->GetItemAt(i);
					if (NULL != pListItem)
					{
						fileInfo* pFile = (fileInfo*)pListItem->GetTag();
						if (NULL != pFile)
						{
							tstring uploadName = pFile->strFileFullName;          //
							DWORD dwFileSize = pFile->dwFileSize;
							map<tstring, DWORD>::iterator itFind = m_sLastSavedProjData.mapFile2Size.find(uploadName);
							if (itFind == m_sLastSavedProjData.mapFile2Size.end())
							{
								bChanged = true;
								break;
							}
							else
							{
								if (itFind->second != dwFileSize)
								{
									bChanged = true;
									break;
								}
							}
						}
					}
				}
			}
		}
	}

	return bChanged;
}

//保存到当前的项目下面去。
void CGroupChatPage::OnBnClickSaveProject()
{
	if (IsSavingFile())
	{
		CMsgBox* pMsgBox = new CMsgBox("提示", "正在保存文件，请等待！", MSGBOX_ICON_WARNING, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}

	CControlUI* pResPackName = (CControlUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl("gm_edit_res_name"));
	tstring strResPackName;
	if (NULL != pResPackName)
	{
		strResPackName = pResPackName->GetText();
		if (strResPackName.empty())
		{
			CMsgBox* pMsgBox = new CMsgBox("新建业务","资源包名称不能为空！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal();
			return;
		}
	}
	m_sLastSavedProjData.strResName = strResPackName;

	CControlUI* pControlMemo = (CControlUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl("gm_richedit_create_newproj_memo"));
	tstring strMemo;
	if (NULL != pControlMemo)
	{
		strMemo = pControlMemo->GetText();
	}
	m_sLastSavedProjData.strMemo = strMemo;

	CComboUI* pComboProperty = (CComboUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl("gm_combo_res_property"));
	tstring strProperty;
	if (NULL != pComboProperty)
	{
		strProperty = pComboProperty->GetText();
	}
	m_sLastSavedProjData.strResProperty = strProperty;
	int nSelectIndex = pComboProperty->GetCurSel();

	///
	int nFileCount = m_pListWithProperty->GetCount();
	if (nFileCount == 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("新建项目","请添加资源文件！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}
	//单个文件size不能大于20M
	for (int i=0; i<nFileCount; i++)
	{
		CContainerUI* pListItem = (CContainerUI*)m_pListWithProperty->GetItemAt(i);
		if (NULL != pListItem)
		{
			fileInfo* pFile = (fileInfo*)pListItem->GetTag();
			if (NULL != pFile)
			{
				DWORD dwSize = pFile->dwFileSize;
				if (dwSize>20*1024*1024)
				{
					CMsgBox* pMsgBox = new CMsgBox("上传文件太大","单个文件大小不能超过20M！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
					pMsgBox->ShowModal();
					return;
				}
			}
		}
	}


	
	
	CHttpClient reqClient;
	tstring strResponse;
	tstring strToken = CController::Instance()->GetToken();

	//获取当前的项目ID
	tstring strUrlNewProj(WEB_SERVER_BASE_URL);
	strUrlNewProj += "business/get-bid-by-rid?rid=";
	tstring strRoomID;
	bool bResu = mainDlg::getInstance()->GetCurrGroupRoomID(strRoomID);
	if (!bResu || strRoomID.empty())
	{
		//当前没有激活的roomID？！
		return;
	}
	
	strUrlNewProj += strRoomID;
	tstring strOutUTF8Projname;
	CChineseCode::GB2312ToUTF_8(strOutUTF8Projname, strUrlNewProj.c_str() , strUrlNewProj.length());
	int nSucc = reqClient.Get(strOutUTF8Projname, strToken, strResponse);

	tstring strProjectID;
	bool bFail = false;
	if (!strResponse.empty())
	{
		//弱判断：
		if (strResponse.find("bid") != -1)
		{
			Json::Reader reader;
			Json::Value root;
			try
			{
				if (reader.parse(strResponse, root))
				{
					strProjectID = root["bid"].asString();
				}
			}
			catch (...)
			{
				CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
			}	
		}
	}

	if (strProjectID.empty())
	{
		CMsgBox* pMsgBox = new CMsgBox("错误","获取项目ID失败！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}


	//2.获取一个PackID
	tstring strFilePackID;
	tstring strUrlGetPackID(WEB_SERVER_BASE_URL);
	strUrlGetPackID += "file/create-file-pack?";
	ostringstream oss;
	oss<<"bid="<<strProjectID<<"&"
		<<"attribute="<<nSelectIndex+1<<"&"
		<<"name="<<strResPackName;
	strUrlGetPackID += oss.str().c_str();
	tstring strOutUTF8;
	CChineseCode::GB2312ToUTF_8(strOutUTF8, strUrlGetPackID.c_str() , strUrlGetPackID.length());

	tstring strGetPackIDResult;
	reqClient.Get(strOutUTF8,strToken, strGetPackIDResult);
	if (!strGetPackIDResult.empty())
	{
		//弱判断：
		if (strGetPackIDResult.find("result") == -1)
		{
			Json::Reader reader;
			Json::Value root;
			try
			{
				if (reader.parse(strGetPackIDResult, root))
				{
					strFilePackID = root["id"].asString();
				}
			}
			catch (...)
			{
				CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
			}	
		}
	
	}

	if (strFilePackID.empty())
	{
		CMsgBox* pMsgBox = new CMsgBox("新建项目","获取文件PackID失败！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}


	if (nFileCount>0)
	{
		if (nFileCount>10)
		{
			CMsgBox* pMsgBox = new CMsgBox("上传文件过多","您选择的文件超过10个，请删除多余文件再上传！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal();
			return;
		}

		for (int i=0; i<nFileCount; i++)
		{
			CContainerUI* pListItem = (CContainerUI*)m_pListWithProperty->GetItemAt(i);
			if (NULL != pListItem)
			{
				fileInfo* pFile = (fileInfo*)pListItem->GetTag();
				if (NULL != pFile)
				{
					tstring uploadName = pFile->strFileFullName;          //
					DWORD sz = pFile->dwFileSize;
					CProgressUI *pProgressBar  = static_cast<CProgressUI*>(m_pPaintManager->FindSubControlByName(pListItem, _T("progress_res_file")));
					if (NULL != pProgressBar)
					{
						// 已经上传的就不再上传了。
						if (pProgressBar->GetValue() == 100)
						{
							continue;
						}
					}
					m_sLastSavedProjData.mapFile2Size.insert(make_pair(uploadName, sz));
					m_sLastSavedProjData.strProjectID = strProjectID;

					uploadFiles(strProjectID, strFilePackID, *pFile, this);
				}
			}
		}
	}
}

bool CGroupChatPage::uploadFiles( tstring& strProjectID, tstring& strPackID, fileInfo& lstFiles, IOssEventBase* pEventHandle)
{
	HANDLE   hThread;  
	unsigned   threadID;   

	sTmpInfo* pTmp = new sTmpInfo;
	pTmp->pFile = &lstFiles;
	pTmp->pThis = (void*)this;
	pTmp->strProjectID = strProjectID;
	pTmp->strPackID = strPackID;
	pTmp->pEventHandle = pEventHandle;
	hThread = (HANDLE)_beginthreadex(NULL,0,uploadThreadFunc,pTmp,0,&threadID);
	CloseHandle(hThread); 
	return true;
}

unsigned  __stdcall  CGroupChatPage::uploadThreadFunc( void*pArguments)
{
	sTmpInfo* pTmp = (sTmpInfo*)pArguments;
	if (NULL != pTmp && NULL != pTmp->pThis)
	{
		((CGroupChatPage*)(pTmp->pThis))->uploadSignalFile(pTmp->strProjectID, pTmp->strPackID, *(pTmp->pFile), pTmp->pEventHandle);
	} 
	return   0;  
}

bool CGroupChatPage::uploadSignalFile( tstring& strProjectID, tstring& strPackID, fileInfo& infoFile, IOssEventBase* pEventHandle)
{

	//2.上传到阿里云
	tstring strEndPoint("oss-cn-hangzhou.aliyuncs.com");
	tstring strAcessID("nHGOOiBnBsJCs04n");
	tstring strAcessIDSecret("Yod3tgIrcqVLpwuIk6yxRJKm56pvXr");
	tstring strBucket("jp-mm");

	ossInterface* pOssControl = new ossInterface;
	pOssControl->AddEventHandler(pEventHandle);
	pOssControl->setAcessEndpoint(strEndPoint);
	pOssControl->setAcessID(strAcessID);
	pOssControl->setAcessIDSecret(strAcessIDSecret);
	pOssControl->setBucketName(strBucket);
	pOssControl->initOSS();

	CHttpClient reqClient;
	tstring strResponse;

	tstring strToken = CController::Instance()->GetToken();

	tstring strOut;
	CChineseCode::GB2312ToUTF_8(strOut, infoFile.strFilePath.c_str(), infoFile.strFilePath.length());

	tstring uploadName = infoFile.strFileFullName;          //
	DWORD sz = infoFile.dwFileSize;

	tstring strObject;
	CChineseCode::GB2312ToUTF_8(strObject, uploadName.c_str(), uploadName.length());

	sTaskItem oneItem;
	oneItem.pOssObject = pOssControl;
	oneItem.bSaving    = true;
	oneItem.strPathFile = infoFile.strFilePath;
	oneItem.strFileName = infoFile.strFileFullName;
	oneItem.hExitEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	ResetEvent(oneItem.hExitEvent);

	::EnterCriticalSection(&m_csUploadFile);
	m_mapSaveFileState.insert(make_pair(oneItem.strPathFile, oneItem));
	::LeaveCriticalSection(&m_csUploadFile);


	int res = pOssControl->uploadPartFromFileEx(strObject, strOut, sz, uploadName/*strObject*/, tstring(_T("")));
	if (res == OSS_SUCCESS)
	{
		//获取阿里云 url
		tstring struploadName = CChineseCode::EncodeUTF8(uploadName);
		tstring strUrl = pOssControl->getFileUrl(struploadName);   //todo encode
		strUrl = UrlEncode(strUrl);

		tstring strFileFullName = infoFile.strFileFullName;
		tstring strParam(WEB_SERVER_BASE_URL); 
		strParam += "file/save-pack-files";
		ostringstream oss;
		oss<<sz;
		tstring strSize = oss.str();


		tstring strConverted;
		if (strFileFullName.find("&") != -1)
		{
			strConverted = UrlEncode(strFileFullName);
		}
		else
		{
			strConverted = strFileFullName;
		}
		tstring strPost = "bid=" + strProjectID + "&pack_id="+strPackID + "&file_datas="
			+"[{" + '"'+"file_name" +'"'+":" + '"'+strConverted.c_str()+'"' +','+"\"file_size\":\""
			+ strSize.c_str() +"\"" +',' + '"'+ "file_type"+'"' +":" + '"'+infoFile.strFileType+'"'+',' 
			+'"'+ "ali_file_url"+'"'+ ":" + '"'+strUrl+'"' + "}]";

		int nResult=0;
		reqClient.Post(strParam,CChineseCode::EncodeUTF8(strPost), strToken, strResponse );	
		if (!strResponse.empty() && strResponse.find("result") != -1)      //保存web数据库失败 
		{
			Json::Reader reader;
			Json::Value root;
			if (reader.parse(strResponse, root))
			{
				nResult = root["result"].asInt();
			}
		}

		if (nResult == 1)
		{
			//跳转到资源包列表页面。并展示出来。
			/*CTabLayoutUI*pGroupChildLayout = static_cast<CTabLayoutUI*>(m_pPaintManager->FindControl("GroupChatFileLayout"));
			if (NULL != pGroupChildLayout)
			{
			pGroupChildLayout->SelectItem(CONST_GROUPCHAT_FILE_LIST_PAGE);
			}*/			
		}
		else
		{
			tstring strError("保存文件资料：[");
			strError += uploadName;
			strError += "]失败！";
			CMsgBox* pMsgBox = new CMsgBox("新建项目",strError.c_str(), MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal();
		}
	}

	pOssControl->RemoveHandler(pEventHandle);

	if (res == OSS_SUCCESS)
	{
		

	}
	else
	{
		//告知线程退出了。
		SetEvent(oneItem.hExitEvent);
	}

	//修改状态。
	::EnterCriticalSection(&m_csUploadFile);
	for (map<tstring, sTaskItem>::iterator it = m_mapSaveFileState.begin();
		it != m_mapSaveFileState.end(); ++it)
	{
		if (_tcsicmp(it->second.strFileName.c_str(), uploadName.c_str()) == 0)
		{
			it->second.bSaving = false;
		}
	}
	::LeaveCriticalSection(&m_csUploadFile);

	//清除最近数据里面的文件列表
	/*for (map<tstring, DWORD>::iterator it2 = m_sLastSavedProjData.mapFile2Size.begin();
		it2 != m_sLastSavedProjData.mapFile2Size.end(); ++it2)
	{
		if (_tcsicmp(it2->first.c_str(), uploadName.c_str()) == 0)
		{
			m_sLastSavedProjData.mapFile2Size.erase(it2);
			break;
		}
	}*/


	if(!IsSavingFile())
	{
		//文件资料包上传成功了：修改为收藏状态，如果是“发送”，那么在消息WM_GROUPPACK_UPLOADED中再给它改回来。。。

		//将资料包状态更新为：1，收藏资料
		CHttpClient reqClient;
		tstring strResponse;
		tstring strToken = CController::Instance()->GetToken();

		//更改pack状态
		tstring strUrlUpdatePack(WEB_SERVER_BASE_URL);
		strUrlUpdatePack += "file/update-pack-status?pack_id=";
		strUrlUpdatePack += strPackID;
		strUrlUpdatePack += "&status=";
		strUrlUpdatePack += "1";

		tstring strOutUTF8Projname;
		CChineseCode::GB2312ToUTF_8(strOutUTF8Projname, strUrlUpdatePack.c_str() , strUrlUpdatePack.length());
		int nSucc = reqClient.Get(strOutUTF8Projname, strToken, strResponse);

		tstring strResponseTemp;
		strResponseTemp = CChineseCode::DecodeUTF8(strResponse);
		if ( strResponse.find("1") != -1)
		{
			//return true;
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("错误","修改该资料包状态失败！");
			pMsgBox->ShowModal();
			//return false;			
		}


		//跳转到资源包列表页面。并展示出来。
		CTabLayoutUI*pGroupChildLayout = static_cast<CTabLayoutUI*>(m_pPaintManager->FindControl("GroupChatFileLayout"));
		if (NULL != pGroupChildLayout)
		{
			pGroupChildLayout->SelectItem(CONST_GROUPCHAT_FILE_LIST_PAGE);
		}	

		//显示资料编辑页面
		tstring strRoomID;
		bool bResu = mainDlg::getInstance()->GetCurrGroupRoomID(strRoomID);
		if (bResu || !strRoomID.empty())
		{
			//当前没有激活的roomID？！
			InitPackFiles(strProjectID, strRoomID);
		}
		

		//发个消息：如果是”发送“资料包
		char* pPackID = new char[100];
		char* pRoomID = new char[100];
		memset(pPackID, 0, 100);
		memset(pRoomID, 0, 100);
		strcpy(pPackID, strPackID.c_str());
		strcpy(pRoomID, strRoomID.c_str());
		g_pMsgWnd->PostMessage(WM_GROUPPACK_UPLOADED, (WPARAM)pRoomID, (LPARAM)pPackID);
	}

	return res == OSS_SUCCESS;
}

void CGroupChatPage::OnProcess(tstring& fileName, int iValue, tstring &strKey)
{
	if (NULL != m_pListWithProperty)
	{
		m_pListWithProperty->SetProgress(fileName, iValue);
	}
}

bool CGroupChatPage::IsSavingFile()
{
	::EnterCriticalSection(&m_csUploadFile);
	bool bSavingfile = false;
	for (map<tstring, sTaskItem>::iterator it = m_mapSaveFileState.begin();
		it != m_mapSaveFileState.end(); ++it)
	{
		if (it->second.bSaving)
		{
			bSavingfile = true;
			break;
		}
	}
	::LeaveCriticalSection(&m_csUploadFile);

	return bSavingfile;
}

bool CGroupChatPage::IsParamEmpty()
{
	CControlUI* pResPackName = (CControlUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl("gm_edit_res_name"));
	tstring strResPackName;
	if (NULL != pResPackName)
	{
		strResPackName = pResPackName->GetText();
		if (strResPackName.empty())
		{
			CMsgBox* pMsgBox = new CMsgBox("新建业务","资源包名称不能为空！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal();
			return false;
		}
	}

	int nFileCount = m_pListWithProperty->GetCount();
	if (nFileCount == 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("新建项目","请添加资源文件！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return false;
	}

	return true;
}

//返回这个Proj下面的所有资料包，客户端需要根据roomID来过滤。。。
void CGroupChatPage::InitPackFiles(tstring& strProjID, tstring& strRoomID)
{
	CHttpClient reqClient;
	tstring strResponse;
	tstring strToken = CController::Instance()->GetToken();

	tstring strUrlGetPackFiles(WEB_SERVER_BASE_URL);
	strUrlGetPackFiles += "file/get-pack-files?";
	ostringstream oss;
	oss<<"bid="<<strProjID;
	strUrlGetPackFiles += oss.str().c_str();
	tstring strOutUTF8;
	CChineseCode::GB2312ToUTF_8(strOutUTF8, strUrlGetPackFiles.c_str() , strUrlGetPackFiles.length());

	tstring strGetPackIDResult;
	reqClient.Get(strOutUTF8,strToken, strGetPackIDResult);
	if (!strGetPackIDResult.empty())
	{
		if(strGetPackIDResult.find("{\"result\":0}") != -1)
		{
			return;
		}

		//弱判断：
		if (strGetPackIDResult.find("id") != -1 && strGetPackIDResult.find("name") != -1 &&
			strGetPackIDResult.find("create_time") != -1 && strGetPackIDResult.find("create_by") != -1 &&
			strGetPackIDResult.find("org_name") != -1)
		{
			//如果有旧数据，就清除
			::EnterCriticalSection(&m_csFLSection);

			Json::Reader reader;
			Json::Value root;
			try
			{
				if (reader.parse(strGetPackIDResult, root))
				{
					int nPackCount = root.size();
					if (nPackCount>0)
					{

						map<tstring, MapPackID2Packs>::iterator itFind = m_mapRoom2PackList.find(strRoomID);
						if (itFind != m_mapRoom2PackList.end())
						{
							m_mapRoom2PackList.erase(itFind);
						}

						int nCount = m_pFilePacketList->GetCount();
						if (nCount>0)
						{
							//memo leak...
							m_pFilePacketList->RemoveAll();
						}

						MapPackID2Packs mapPacks;
						//增加新的数据
						for (int  i=0;  i<nPackCount;  i++)
						{
							tstring strPackID;
							CChineseCode::UTF_8ToGB2312(strPackID, root[i]["id"].asString().c_str(), root[i]["id"].asString().length());	

							tstring strPackName;
							CChineseCode::UTF_8ToGB2312(strPackName, root[i]["name"].asString().c_str(), root[i]["name"].asString().length());	
							tstring strPackTime ;
							CChineseCode::UTF_8ToGB2312(strPackTime, root[i]["create_time"].asString().c_str(),root[i]["create_time"].asString().length());
							tstring strHMMTime = strPackTime.substr(11, strPackTime.length()-11);//hardcode,只去了时间没有日期
							tstring struid   = root[i]["create_by"].asString();
							tstring strSender;
							CChineseCode::UTF_8ToGB2312(strSender, root[i]["create_by"].asString().c_str(), root[i]["create_by"].asString().length());

							tstring strAtrtibute;
							CChineseCode::UTF_8ToGB2312(strAtrtibute, root[i]["attribute"].asString().c_str(), root[i]["attribute"].asString().length());
							//包的状态
							int nStatus = root[i]["status"].asInt();

							tstring strOrgname;
							CChineseCode::UTF_8ToGB2312(strOrgname, root[i]["org_name"].asString().c_str(), root[i]["org_name"].asString().length());

							tstring strOrgID;
							strOrgID = root[i]["oid"].asString();

							tstring strCreaterPhone;
							CChineseCode::UTF_8ToGB2312(strCreaterPhone, root[i]["cell_phone"].asString().c_str(),root[i]["cell_phone"].asString().length());

							sPackItem newPack;
							newPack.strPackID = strPackID;
							newPack.strPackName = strPackName;
							newPack.strPackTime = strHMMTime/*strPackTime*/;
							newPack.strPackProp = strAtrtibute;
							newPack.nPackStatus = nStatus;
							newPack.strPackCreaterPhone = strCreaterPhone;
							newPack.strOrgID = strOrgID.empty()? DEF_ORGID_MYFRIEND : strOrgID;
							newPack.strOrgName = strOrgname.empty()? ORGANIZATION_MYFRIEND: strOrgname;

							//get vcard info.
							sUserVCard oneCard;
							bool bHaveCard = false;
							tstring strSenderAccount = strCreaterPhone+MMPC_ACCOUNT;
							if(CController::Instance()->GetUserInfoMgr()->GetUserVCard(strSenderAccount, oneCard))
							{
								bHaveCard = true;
							}
							if (!bHaveCard)
							{
								GetDBTaskModule()->LoadUserVCard(strSenderAccount, oneCard);
							}

							tstring strShowName = oneCard.strUserNickname.empty()? strCreaterPhone : oneCard.strUserNickname;
							newPack.strPackSender = strOrgname.empty() ? strShowName : strOrgname+"-"+strShowName;

							int nFileCount = root[i]["file_lists"].size();
							int nPackFileSize = 0;
							for (int j=0; j<nFileCount; j++)
							{
								sPackFileItem oneFile;
								oneFile.strPackID = newPack.strPackID;
								oneFile.strPackName = newPack.strPackName;

								CChineseCode::UTF_8ToGB2312(oneFile.strFileName, root[i]["file_lists"][j]["file_name"].asString().c_str(), root[i]["file_lists"][j]["file_name"].asString().length());				
								oneFile.strFileSizeAccurate = root[i]["file_lists"][j]["file_size"].asString().c_str();
								int nFileSize = _tcstoul(oneFile.strFileSizeAccurate.c_str(), NULL, 10);
								nPackFileSize += nFileSize;
								oneFile.strFileSizeShow = GetFileSizeFormat(nFileSize);
								oneFile.strFileType  = root[i]["file_lists"][j]["file_type"].asString();
								oneFile.strFileUrl   = root[i]["file_lists"][j]["ali_file_url"].asString();
								tstring strCreater = root[i]["file_lists"][j]["create_by"].asString();
								newPack.listFile.push_back(oneFile);
							}

							newPack.strPackSize = GetFileSizeFormat(nPackFileSize);

							//添加一个Pack父节点
							FriendListItemInfoEx item;
							item.folder = true;
							item.strPackIcon = DEF_PACK_ICON;
							item.strOrgID = strOrgID.empty()? DEF_ORGID_MYFRIEND : strOrgID;
							item.strOrgName = strPackName.empty()?ORGANIZATION_MYFRIEND : strOrgname;
							item.strPackID  = newPack.strPackID;
							item.strPackName = newPack.strPackName;
							item.strPackProp = newPack.strPackProp;
							item.nPackStatus = newPack.nPackStatus;
							item.strPackSender = newPack.strPackSender;
							item.strPackSize = newPack.strPackSize;
							item.strPackTime = newPack.strPackTime;
							item.strPackCreater = newPack.strPackCreaterPhone;
							item.itemId = GetNextItemID();

							m_pFilePacketList->SetExcuteDuty(IsAbleToExecute());
							Node* pParentNode = m_pFilePacketList->AddNode(item, NULL);
							if (newPack.listFile.size()>0)
							{
								for (list<sPackFileItem>::iterator it = newPack.listFile.begin(); it != newPack.listFile.end(); ++it)
								{
									sPackFileItem& oneFile = *it;
									//创建文件子节点
									FriendListItemInfoEx itemLeaf;
									itemLeaf.folder = false;
									itemLeaf.itemId = GetNextItemID();
									itemLeaf.strOrgID = strOrgID.empty()? DEF_ORGID_MYFRIEND : strOrgID;
									itemLeaf.strOrgName = strPackName.empty()?ORGANIZATION_MYFRIEND : strOrgname;
									itemLeaf.strPackID = newPack.strPackID;
									itemLeaf.strPackName = newPack.strPackName;
									itemLeaf.strFileIcon = DEF_AVATAR_NOR;
									itemLeaf.strFileName= oneFile.strFileName;
									itemLeaf.strFileSize = oneFile.strFileSizeShow;
									itemLeaf.strFileSizeAccurate = oneFile.strFileSizeAccurate;
									itemLeaf.strFileType = oneFile.strFileType;
									itemLeaf.strFileUrl = oneFile.strFileUrl;
									m_pFilePacketList->AddNode(itemLeaf,pParentNode);
								}
							}

							mapPacks.insert(make_pair(strPackID, newPack));
						}

						//缓存下来
						m_mapRoom2PackList.insert(make_pair(strRoomID, mapPacks));
					}
				}
			}
			catch (...)
			{
				CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
			}	

			::LeaveCriticalSection(&m_csFLSection);
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("错误", "获取资料包列表失败了！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
		}
		
	}
}

void CGroupChatPage::HandleItemClickEvent( TNotifyUI& msg )
{
	tstring strName = msg.pSender->GetName();

	if (0 == msg.pSender->GetName().Find("ResListItem_")
		&& (m_pFilePacketList != NULL) 
		&&  m_pFilePacketList->GetItemIndex(msg.pSender) != -1)
	{
		if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
		{
			Node* node = (Node*)msg.pSender->GetTag();
			tstring strAliUrl = node->data().strAliUrl;
			if (!strAliUrl.empty())
			{

			}
		}
	}
}

bool CGroupChatPage::downloadFiles( fileInfo& lstFiles, IOssEventBase* pEventHandle)
{
	HANDLE   hThread;  
	unsigned   threadID;   
	sTmpInfo* pTmp = new sTmpInfo;
	pTmp->pFile = &lstFiles;
	pTmp->pThis = (void*)this;
	pTmp->pEventHandle = pEventHandle;
	hThread = (HANDLE)_beginthreadex(NULL,0, downloadThreadFunc,pTmp,0,&threadID);
	CloseHandle(hThread); 
	return true;
}

unsigned  __stdcall CGroupChatPage::downloadThreadFunc( void*pArguments )
{
	sTmpInfo* pTmp = (sTmpInfo*)pArguments;
	if (NULL != pTmp && NULL != pTmp->pThis)
	{
		((CGroupChatPage*)(pTmp->pThis))->downloadSignalFile(*(pTmp->pFile), pTmp->pEventHandle);
	} 
	return   0;  
}

bool CGroupChatPage::downloadSignalFile( fileInfo& infoFile, IOssEventBase* pEventHandle)
{
	tstring strEndPoint("oss-cn-hangzhou.aliyuncs.com");
	tstring strAcessID("nHGOOiBnBsJCs04n");
	tstring strAcessIDSecret("Yod3tgIrcqVLpwuIk6yxRJKm56pvXr");
	tstring strBucket(" jp-mm");
	int res = 0;
	ossInterface ossControl;
	ossControl.AddEventHandler(pEventHandle);
	ossControl.setAcessEndpoint(strEndPoint);
	ossControl.setAcessID(strAcessID);
	ossControl.setAcessIDSecret(strAcessIDSecret);
	ossControl.setBucketName(strBucket);
	ossControl.initOSS();


	res = ossControl.downloadObjectByUrl(infoFile.strFileUrl, 
		CChineseCode::EncodeUTF8(infoFile.strFilePath), 
		infoFile.dwFileSize, 
		infoFile.strFileFullName, 
		infoFile.strKey, 
		infoFile.stropposite);
	bool bSucc = res == OSS_SUCCESS;
	if (bSucc)
	{
		ShellExecute(NULL, NULL, infoFile.strFilePath.c_str(), NULL,NULL,SW_SHOWNORMAL);
	}

	fileInfo* pFileInfo = &infoFile;
	if (NULL != pFileInfo)
	{
		delete pFileInfo;
	}

	return bSucc;
}

bool CGroupChatPage::UpdatePackStatus(CControlUI* pSender, tstring& strStatus)
{
	CListContainerElementUI* pElement = static_cast<CListContainerElementUI*>(pSender->GetParent()->GetParent()->GetParent());
	if (NULL != pElement)
	{
		Node* pNode = (Node*)pElement->GetTag();
		tstring strPackID = pNode->data().strPackID;

		CHttpClient reqClient;
		tstring strResponse;
		tstring strToken = CController::Instance()->GetToken();

		//更改pack状态
		tstring strUrlUpdatePack(WEB_SERVER_BASE_URL);
		strUrlUpdatePack += "file/update-pack-status?pack_id=";
		strUrlUpdatePack += strPackID;
		strUrlUpdatePack += "&status=";
		strUrlUpdatePack += strStatus;

		tstring strOutUTF8Projname;
		CChineseCode::GB2312ToUTF_8(strOutUTF8Projname, strUrlUpdatePack.c_str() , strUrlUpdatePack.length());
		int nSucc = reqClient.Get(strOutUTF8Projname, strToken, strResponse);
		if ( strResponse.find("1") != -1)
		{
			return true;
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("错误","修改该资料包状态失败！");
			pMsgBox->ShowModal();
			return false;			
		}
	}
}

bool CGroupChatPage::IsAbleToExecute()
{
	if (m_vecMyDutys.size()>0)
	{
		for (int i=0; i<m_vecMyDutys.size(); i++)
		{
			tstring& strDuty = m_vecMyDutys[i];
			if (_tcsicmp(strDuty.c_str(), "执行") == 0)
			{
				return true;
			}
		}
	}
	
	return false;
}
void CGroupChatPage::InitMyDutys()
{
	tstring strMyAccount = CController::Instance()->GetXmppClient()->jid().bare();
	CUserInfoManager* pUsrMgr = CController::Instance()->GetUserInfoMgr();
	sRosterItemLocal myRoster;
	pUsrMgr->GetOneUserRoster(strMyAccount, myRoster);
	GetDuty(myRoster.strOrgID, strMyAccount, m_vecMyDutys);
}

//签约包的状态改变了，更新cache。保证refreshPackList的数据正确。
void CGroupChatPage::UpdatePackCache(tstring& strRoomID, tstring& strPackID, tstring& strNewStatus)
{
	int nNewStatue=0;
	if (_tcsicmp("已签约", strNewStatus.c_str()) == 0)
	{
		nNewStatue = 3;
	}
	else if (_tcsicmp("已拒绝", strNewStatus.c_str()) == 0)
	{
		nNewStatue = 4;
	}

	::EnterCriticalSection(&m_csFLSection);
	map<tstring, MapPackID2Packs>::iterator itFind = m_mapRoom2PackList.find(strRoomID);
	if (itFind != m_mapRoom2PackList.end())
	{
		//一个Room下的Packet列表
		MapPackID2Packs& mapPacks = itFind->second;
		for (MapPackID2Packs::iterator it = mapPacks.begin(); it != mapPacks.end(); ++it)
		{
			sPackItem& onePack = it->second;
			if (_tcsicmp(onePack.strPackID.c_str(), strPackID.c_str()) == 0)
			{
				onePack.nPackStatus = nNewStatue;
			}
		}
	}
	::LeaveCriticalSection(&m_csFLSection);
}

bool CGroupChatPage::IsPackExist(tstring& strRoomID, tstring& strPackID)
{
	bool bExist = false;
	::EnterCriticalSection(&m_csFLSection);
	map<tstring, MapPackID2Packs>::iterator itFind = m_mapRoom2PackList.find(strRoomID);
	if (itFind != m_mapRoom2PackList.end())
	{
		//一个Room下的Packet列表
		MapPackID2Packs& mapPacks = itFind->second;
		for (MapPackID2Packs::iterator it = mapPacks.begin(); it != mapPacks.end(); ++it)
		{
			sPackItem& onePack = it->second;
			if (_tcsicmp(onePack.strPackID.c_str(), strPackID.c_str()) == 0)
			{
				bExist = true;
				break;
			}
		}
	}
	::LeaveCriticalSection(&m_csFLSection);

	return bExist;
}

void CGroupChatPage::InsertFirstPack(tstring& strRoom,tstring& strPackID)
{
	//获取这个pack信息
	sPackItem OnePack;
	OnePack.strPackID = strPackID;

	CHttpClient reqClient;
	tstring strResponse;
	tstring strUrlGetPackFiles(WEB_SERVER_SITE_URL);
	strUrlGetPackFiles += "get-file-list-by-pack?";

	ostringstream oss;
	oss<<"pack_id="<<strPackID.c_str();
	strUrlGetPackFiles += oss.str().c_str();

	tstring strOutUTF8;
	CChineseCode::GB2312ToUTF_8(strOutUTF8, strUrlGetPackFiles.c_str() , strUrlGetPackFiles.length());
	tstring strGetPackIDResult;
	reqClient.Get(strOutUTF8, strGetPackIDResult);
	if (!strGetPackIDResult.empty())
	{
		//若判断
		if (strGetPackIDResult.find("name") != -1 && strGetPackIDResult.find("attribute")!=-1
			&&strGetPackIDResult.find("create_time") != -1 && strGetPackIDResult.find("create_by") != -1)
		{
			Json::Reader reader;
			Json::Value root;
			try
			{
				if (reader.parse(strGetPackIDResult, root))
				{
					CChineseCode::UTF_8ToGB2312(OnePack.strPackName, root["name"].asString().c_str(), root["name"].asString().length());
					tstring strPackPropry;
					CChineseCode::UTF_8ToGB2312(OnePack.strPackProp, root["attribute"].asString().c_str(), root["attribute"].asString().length());

					tstring strPackTime ;
					CChineseCode::UTF_8ToGB2312(strPackTime, root["create_time"].asString().c_str(),root["create_time"].asString().length());
					tstring strHMMTime = strPackTime.substr(11, strPackTime.length()-11);//hardcode,只去了时间没有日期
					OnePack.strPackTime = strHMMTime;

					tstring strAtrtibute;
					CChineseCode::UTF_8ToGB2312(strAtrtibute, root["attribute"].asString().c_str(), root["attribute"].asString().length());
					//包的状态
					int nStatus = root["status"].asInt();
					OnePack.nPackStatus = nStatus;

					tstring strCreaterPhone;
					CChineseCode::UTF_8ToGB2312(strCreaterPhone, root["cell_phone"].asString().c_str(),root["cell_phone"].asString().length());
					OnePack.strPackCreaterPhone = strCreaterPhone;
					
					//根据手机号从本地查询到这个人的昵称，如果没有昵称，就显示手机号
					tstring strSenderAccount = strCreaterPhone+MMPC_ACCOUNT;
					sUserVCard oneCard;
					bool bHaveCard = false;
					bHaveCard = CController::Instance()->GetUserInfoMgr()->GetUserVCard(strSenderAccount, oneCard);
					//如果没有找到，就去本地数据库去找
					if (!bHaveCard)
					{
						GetDBTaskModule()->LoadUserVCard(strSenderAccount, oneCard);
					}

					tstring strShowName = oneCard.strUserNickname.empty()? strCreaterPhone : oneCard.strUserNickname;

					//考虑到昵称会变化，所以从本地来读取。
					//tstring strSender;
					//CChineseCode::UTF_8ToGB2312(strSender, root["create_by"].asString().c_str(), root["create_by"].asString().length());
					tstring strOrgname;
					CChineseCode::UTF_8ToGB2312(strOrgname, root["org_name"].asString().c_str(), root["org_name"].asString().length());
					OnePack.strPackSender = strOrgname.empty() ? strShowName : strOrgname+"-"+strShowName;

					int nFileCount = root["file_lists"].size();
					int nPackFileSize = 0;
					for (int j=0; j<nFileCount; j++)
					{
						sPackFileItem oneFile;
						oneFile.strPackID = OnePack.strPackID;
						oneFile.strPackName = OnePack.strPackName;

						CChineseCode::UTF_8ToGB2312(oneFile.strFileName, root["file_lists"][j]["file_name"].asString().c_str(), root["file_lists"][j]["file_name"].asString().length());				
						oneFile.strFileSizeAccurate = root["file_lists"][j]["file_size"].asString().c_str();
						int nFileSize = _tcstoul(oneFile.strFileSizeAccurate.c_str(), NULL, 10);
						nPackFileSize += nFileSize;
						oneFile.strFileSizeShow = GetFileSizeFormat(nFileSize);
						oneFile.strFileType  = root["file_lists"][j]["file_type"].asString();
						oneFile.strFileUrl   = root["file_lists"][j]["ali_file_url"].asString();
						OnePack.listFile.push_back(oneFile);
					}
					OnePack.strPackSize = GetFileSizeFormat(nPackFileSize);

					
					//发送之前，更新包的状态为-1
					tstring strStatus("-1");
					CWebClient::updatePackStatus(OnePack.strPackID, strStatus);

					//发送这个消息到群里面去。。
					{
						ostringstream os;
						CFileHelper helper;
						os<<"#COMMAND#"<<"type="<<helper.GetDescription(enmCmd_GroupPackFile).c_str()<<","
							<<"{"
							<<"\"createBy\":\""<<OnePack.strPackCreaterPhone<<"\","
							<<"\"packid\":\""<<OnePack.strPackID<<"\","
							<<"\"attribute\":\""<<GetAttibuteValue(OnePack.strPackProp)<<"\","
							<<"\"packname\":\""<<OnePack.strPackName<<"\","
							<<"\"aliNormalFileList\":[";

						int nFileCountEx = OnePack.listFile.size();
						int nIdx=0;
						for (list<sPackFileItem>::iterator it = OnePack.listFile.begin(); 
							it != OnePack.listFile.end(); ++it)
						{
							os<<"{"
								<<"\"file_name\":\""<<it->strFileName<<"\","
								<<"\"file_type\":\""<<it->strFileType<<"\","
								<<"\"file_size\":\""<<it->strFileSizeAccurate<<"\","
								<<"\"ali_file_url\":\""<<it->strFileUrl<<"\""
								<<"}";

							nIdx++;
							if (nIdx <nFileCountEx)
							{
								os<<",";
							}
						}

						os<<"]}";

						CController::Instance()->GetRoomManager()->SendMessage(strRoom, os.str());
					}
				}
			}
			catch (...)
			{
				CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
			}
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("错误", "获取资料包列表失败！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
		}

	}
}

void CGroupChatPage::AddNewPackItem(tstring& strRoomID, tstring& strPackID)
{
	//////////////////////////////////////////////////////////////////////////
	//pack列表加入一个新的Item
	//获取这个pack信息
	sPackItem OnePack;//new sPackItem;
	OnePack.strPackID = strPackID;

	CHttpClient reqClient;
	tstring strResponse;
	tstring strUrlGetPackFiles(WEB_SERVER_SITE_URL);
	strUrlGetPackFiles += "get-file-list-by-pack?";

	ostringstream oss;
	oss<<"pack_id="<<strPackID.c_str();
	strUrlGetPackFiles += oss.str().c_str();

	tstring strOutUTF8;
	CChineseCode::GB2312ToUTF_8(strOutUTF8, strUrlGetPackFiles.c_str() , strUrlGetPackFiles.length());
	tstring strGetPackIDResult;
	reqClient.Get(strOutUTF8, strGetPackIDResult);
	if (!strGetPackIDResult.empty())
	{
		//若判断
		if (strGetPackIDResult.find("name") != -1 && strGetPackIDResult.find("attribute")!=-1
			&&strGetPackIDResult.find("create_time") != -1 && strGetPackIDResult.find("create_by") != -1)
		{
			Json::Reader reader;
			Json::Value root;
			try
			{
				if (reader.parse(strGetPackIDResult, root))
				{
					CChineseCode::UTF_8ToGB2312(OnePack.strPackName, root["name"].asString().c_str(), root["name"].asString().length());
					tstring strPackPropry;
					CChineseCode::UTF_8ToGB2312(OnePack.strPackProp, root["attribute"].asString().c_str(), root["attribute"].asString().length());

					tstring strPackTime ;
					CChineseCode::UTF_8ToGB2312(strPackTime, root["create_time"].asString().c_str(),root["create_time"].asString().length());
					tstring strHMMTime = strPackTime.substr(11, strPackTime.length()-11);//hardcode,只去了时间没有日期
					OnePack.strPackTime = strHMMTime;

					tstring strAtrtibute;
					CChineseCode::UTF_8ToGB2312(strAtrtibute, root["attribute"].asString().c_str(), root["attribute"].asString().length());
					//包的状态
					int nStatus = root["status"].asInt();
					OnePack.nPackStatus = nStatus;

					//通过本地缓存来查询
					//tstring struid   = root["create_by"].asString();
					//tstring strSender;
					//CChineseCode::UTF_8ToGB2312(strSender, root["create_by"].asString().c_str(), root["create_by"].asString().length());

					
		
					tstring strCreaterPhone;
					CChineseCode::UTF_8ToGB2312(strCreaterPhone, root["cell_phone"].asString().c_str(),root["cell_phone"].asString().length());
					OnePack.strPackCreaterPhone = strCreaterPhone;

					//查询这个人的昵称
					tstring strSenderAccount = strCreaterPhone+MMPC_ACCOUNT;
					sUserVCard oneCard;
					bool bHaveCard =false;
					bHaveCard = CController::Instance()->GetUserInfoMgr()->GetUserVCard(strSenderAccount, oneCard);
					if (!bHaveCard)
					{
						GetDBTaskModule()->LoadUserVCard(strSenderAccount, oneCard);
					}
					tstring strShowName = oneCard.strUserNickname.empty()? strCreaterPhone : oneCard.strUserNickname;

					//赋值机构信息。
					OnePack.strOrgID = root["oid"].asString();
					tstring strOrgname;
					CChineseCode::UTF_8ToGB2312(strOrgname, root["org_name"].asString().c_str(), root["org_name"].asString().length());
					OnePack.strOrgName = strOrgname;
					OnePack.strPackSender = strOrgname.empty()? strShowName : strOrgname+"-"+strShowName;


					int nFileCount = root["file_lists"].size();
					int nPackFileSize = 0;
					for (int j=0; j<nFileCount; j++)
					{
						sPackFileItem oneFile;
						oneFile.strPackID = OnePack.strPackID;
						oneFile.strPackName = OnePack.strPackName;

						CChineseCode::UTF_8ToGB2312(oneFile.strFileName, root["file_lists"][j]["file_name"].asString().c_str(), root["file_lists"][j]["file_name"].asString().length());				
						oneFile.strFileSizeAccurate = root["file_lists"][j]["file_size"].asString().c_str();
						int nFileSize = _tcstoul(oneFile.strFileSizeAccurate.c_str(), NULL, 10);
						nPackFileSize += nFileSize;
						oneFile.strFileSizeShow = GetFileSizeFormat(nFileSize);
						oneFile.strFileType  = root["file_lists"][j]["file_type"].asString();
						oneFile.strFileUrl   = root["file_lists"][j]["ali_file_url"].asString();
						OnePack.listFile.push_back(oneFile);
					}
					OnePack.strPackSize = GetFileSizeFormat(nPackFileSize);

					//缓存下来
					map<tstring, MapPackID2Packs>::iterator itFind = m_mapRoom2PackList.find(strRoomID);
					if (itFind != m_mapRoom2PackList.end())
					{
						itFind->second.insert(make_pair(strPackID, OnePack));
					}
					//有可能是第一次收到的离线消息的资料包信息===》如果全部重新拉取，会造成web很卡。。
					else
					{
						MapPackID2Packs mapPacks;
						mapPacks.insert(make_pair(strPackID, OnePack));
						m_mapRoom2PackList.insert(make_pair(strRoomID, mapPacks));
					}
				}
			}
			catch (...)
			{
				CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
			}
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("错误", "获取资料包列表失败！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal(NULL);
		}
		
	}
}

void CGroupChatPage::SetSendMsgKeyMode(eSendMsgMode mode)
{
	m_eSendMode  = mode;
	GetDBTaskModule()->SaveSendMsgHotKey(mode);
}

bool CGroupChatPage::IsEnterSendMode()
{
	return m_eSendMode == eEnterKey;
}

void CGroupChatPage::SendGroupMessage()
{
	OnSendMessage();
}

void CGroupChatPage::SendGroupMessage(tstring& strContent)
{
	//send mesage
	if (NULL != CController::Instance()->GetRoomManager())
	{
		//check if empty.
		if (strContent.length() == 0)
		{
			return;
		}
		if (!m_strLastRoomID.empty())
		{
			CController::Instance()->GetRoomManager()->SendMessage(m_strLastRoomID, strContent);
		}
	}
}

bool CGroupChatPage::CheckIfPackRequest(tstring& strContent, sPackItem& onePack)
{
	bool bGroupFileMsg = false;
	if (strContent.find("#COMMAND#") != -1)
	{
		//#COMMAND#type=packRequest,{org_name,createBy,createTime,desc,packid,packname,attribute,aliNormalFileList[{file_name,file_type,file_size,ali_file_url,file_path}]}
		if (strContent.find("packRequest") != -1)
		{
			int index = strContent.find("{");
			if (index == -1)
			{
				return false;
			}

			tstring strJsonObj = strContent.substr(index, strContent.length()-index);
			//tstring strUTF8 ;
			//CChineseCode::GB2312ToUTF_8(strUTF8, strJsonObj.c_str(), strJsonObj.length());

			if (index > 0)
			{
				if (strJsonObj.find("packid") != -1 
					&& strJsonObj.find("createBy") != -1 
					&& strJsonObj.find("packname") != -1
					&& strJsonObj.find("attribute") != -1)
				{
					Json::Reader reader;
					Json::Value root;
					try
					{
						if (reader.parse(strJsonObj, root))
						{
							onePack.strPackID = root["packid"].asString();
							onePack.strPackCreaterPhone = root["createBy"].asString();
							onePack.strPackName = root["packname"].asString();
							tstring strAttributeValue = root["attribute"].asString();
							onePack.strPackProp = GetAttibuteDescription(_tcstoul(strAttributeValue.c_str(), NULL, 10));
							
							//拼接显示在资料包展示区的发送方信息。
							tstring strSenderAccount = onePack.strPackCreaterPhone+MMPC_ACCOUNT;
							sUserVCard oneCard;
							bool bHaveCard = false;
							if(CController::Instance()->GetUserInfoMgr()->GetUserVCard(strSenderAccount, oneCard))
							{
								bHaveCard = true;
							}
							if (!bHaveCard)
							{
								GetDBTaskModule()->LoadUserVCard(strSenderAccount, oneCard);
							}

							tstring strShowName = oneCard.strUserNickname.empty()? onePack.strPackCreaterPhone : oneCard.strUserNickname;
							tstring strOrgName = CController::Instance()->GetUserInfoMgr()->GetOrgName(strSenderAccount);
							onePack.strPackSender = strOrgName.empty() ? strShowName : strOrgName+"-"+strShowName;


							int nPackSize = 0;
							int nFileCount = root["aliNormalFileList"].size();
							for (int i=0; i<nFileCount; i++)
							{
								sPackFileItem oneFile;
								oneFile.strPackID = onePack.strPackID;
								oneFile.strPackName =  onePack.strPackName;

								oneFile.strFileName = root["aliNormalFileList"][i]["file_name"].asString();
								oneFile.strFileType = root["aliNormalFileList"][i]["file_type"].asString();
								oneFile.strFileSizeAccurate = root["aliNormalFileList"][i]["file_size"].asString();
								oneFile.strFileUrl = root["aliNormalFileList"][i]["ali_file_url"].asString();

								onePack.listFile.push_back(oneFile);

								int nFileSize = _tcstoul(oneFile.strFileSizeAccurate.c_str(), NULL, 10);
								nPackSize += nFileSize;
							}

							onePack.strPackSize = GetFileSizeFormat(nPackSize);

							bGroupFileMsg = true;
							//去web 拉取文件列表。并在ChatList插入文件清单列表
						}
					}
					catch (...)
					{
						CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s",strJsonObj.c_str());	
					}
				}
				else
				{
					CMsgBox* pMsgBox = new CMsgBox("错误", "命令消息格式错误：packRequest", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
					pMsgBox->ShowModal(NULL);
				}
			}
		}
	}

	return bGroupFileMsg;
}

bool CGroupChatPage::CheckIfPackDownloadReceipt(tstring& strContent, tstring& sPackName)
{
	bool bGroupPackDownLoadReceiptMsg = false;

	tstring sSignal = "#COMMAND#type=packdownloaded,";
	//from:查看资料包的人的电话号码		
	if ( strContent.find(sSignal)!=tstring::npos )//确定是查看资料包
	{
		tstring sJson = strContent.substr(sSignal.length());
		Json::Reader reader;
		Json::Value root;
		try
		{
			if (reader.parse(sJson, root))
			{
				sPackName = root["packname"].asString();
				bGroupPackDownLoadReceiptMsg = true;
			}
		}
		catch (...)
		{
			CLogger::GetInstance()->PrintErrLog( "parse group packdownload receipt exception: %s", sJson.c_str());	
		}	
	}
	return bGroupPackDownLoadReceiptMsg;
}

void CGroupChatPage::OnSendContract()
{
	vector<tstring> info;
	// 合约名称
	CControlUI* pContractName = (CControlUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl(EDIT_CONTRACT_PAGE_NAME));
	tstring strContractName;
	if (NULL != pContractName)
	{
		strContractName = pContractName->GetText();
		if (strContractName.empty())
		{
			CMsgBox* pMsgBox = new CMsgBox("发送合约","合约名称不能为空！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal();
			return;
		}
	}
	// 收款方/付款方
	tstring strIdentity = CONTRACT_IDENTITY_PAYEE;
	COptionUI* OptionIn = static_cast<COptionUI*>(m_pPaintManager->FindControl(RADIO_CONTRACT_PAGE_IN));
	if(NULL != OptionIn)
	{
		if(OptionIn->IsSelected())
			strIdentity = CONTRACT_IDENTITY_PAYEE;
	}

	COptionUI* OptionOut = static_cast<COptionUI*>(m_pPaintManager->FindControl(RADIO_CONTRACT_PAGE_OUT));
	if(NULL != OptionOut)
	{
		if(OptionOut->IsSelected())
			strIdentity = CONTRACT_IDENTITY_DRAWEE;
	}

	// 合约金额
	CControlUI* pContractMoney = (CControlUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl(EDIT_CONTRACT_PAGE_MONEY));
	tstring strContractMoney;
	if (NULL != pContractMoney)
	{
		strContractMoney = pContractMoney->GetText();
		if(strContractMoney.empty())
		{
			CMsgBox* pMsgBox = new CMsgBox("发送合约","请输入正确的合约金额！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
			pMsgBox->ShowModal();
			return;
		}
	}

	// 付款方式
	tstring strPayType = CONTRACT_PAY_TYPE_AGREE;
	COptionUI* OptionNone = static_cast<COptionUI*>(m_pPaintManager->FindControl(RADIO_CONTRACT_PAGE_NONE));
	if(NULL != OptionNone)
	{
		if(OptionNone->IsSelected())
			strPayType = CONTRACT_PAY_TYPE_NONE;
	}

	COptionUI* OptionSure = static_cast<COptionUI*>(m_pPaintManager->FindControl(RADIO_CONTRACT_PAGE_SURE));
	if(NULL != OptionSure)
	{
		if(OptionSure->IsSelected())
			strPayType = CONTRACT_PAY_TYPE_SURE;
	}

	COptionUI* OptionAgree = static_cast<COptionUI*>(m_pPaintManager->FindControl(RADIO_CONTRACT_PAGE_AGREE));
	if(NULL != OptionAgree)
	{
		if(OptionAgree->IsSelected())
			strPayType = CONTRACT_PAY_TYPE_AGREE;
	}

	//备注
	CControlUI* pControlMemo = (CControlUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl(REDIT_CONTRACT_PAGE_INFO));
	tstring strMemo;
	if (NULL != pControlMemo)
	{
		strMemo = pControlMemo->GetText();
	}

	info.push_back(strContractName);
	info.push_back(strIdentity);
	info.push_back(strContractMoney);
	info.push_back(strPayType);
	tstring strOrgID;
	mainDlg::getInstance()->GetCurrGroupOrgID(strOrgID);
	info.push_back(strOrgID);
	info.push_back(strMemo);
//	info.push_back(_T(""));

	///
	int nFileCount = m_pContractFiles->GetCount();
	if (nFileCount == 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("发送合约","您没有添加合约文件！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}

	CHttpClient reqClient;
	tstring strResponse;
	tstring strToken = CController::Instance()->GetToken();
	//获取当前的项目ID
	tstring strUrlNewProj(WEB_SERVER_BASE_URL);
	strUrlNewProj += "business/get-bid-by-rid?rid=";
	tstring strRoomID;
	bool bResu = mainDlg::getInstance()->GetCurrGroupRoomID(strRoomID);
	if (!bResu || strRoomID.empty())
	{
		//当前没有激活的roomID？！
		return;
	}

	strUrlNewProj += strRoomID;
	tstring strOutUTF8Projname;
	CChineseCode::GB2312ToUTF_8(strOutUTF8Projname, strUrlNewProj.c_str() , strUrlNewProj.length());
	int nSucc = reqClient.Get(strOutUTF8Projname, strToken, strResponse);

	tstring strProjectID;
	bool bFail = false;
	if (!strResponse.empty())
	{
		//弱判断：
		if (strResponse.find("bid") != -1)
		{
			Json::Reader reader;
			Json::Value root;
			try
			{
				if (reader.parse(strResponse, root))
				{
					strProjectID = root["bid"].asString();
				}
			}
			catch (...)
			{
				CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strResponse.c_str());	
			}	
		}
	}	

	if (strProjectID.empty())
	{
		CMsgBox* pMsgBox = new CMsgBox("错误","获取项目ID失败！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}

	info.push_back(strProjectID);

	//获取当前合约id
	tstring strRespons;
	CWebClient::addContract(info,strRespons);

	tstring strContractId;
	if (!strRespons.empty())
	{
		//弱判断：
		if (strRespons.find("id") != -1)
		{
			Json::Reader reader;
			Json::Value root;
			try
			{
				if (reader.parse(strRespons, root))
				{
					strContractId = root["id"].asString();
				}
			}
			catch (...)
			{
				CLogger::GetInstance()->PrintErrLog( "parse http Response exception: %s", strRespons.c_str());	
			}	
		}
	}

	if(strContractId.empty())
	{
		CMsgBox* pMsgBox = new CMsgBox("错误","获取合约ID失败！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return;
	}

	if (nFileCount>0)
	{
		for (int i=0; i<nFileCount; i++)
		{
			CContainerUI* pListItem = (CContainerUI*)m_pContractFiles->GetItemAt(i);
			if (NULL != pListItem)
			{
				fileInfo* pFile = (fileInfo*)pListItem->GetTag();
				if (NULL != pFile)
				{
					tstring uploadName = pFile->strFileFullName;          //
					DWORD sz = pFile->dwFileSize;
					CProgressUI*pProgressBar  = static_cast<CProgressUI*>(m_pPaintManager->FindSubControlByName(pListItem, _T("progress_res_file")));
					if (NULL != pProgressBar)
					{
						// 已经上传的就不再上传了。
						if (pProgressBar->GetValue() == 100)
						{
							continue;
						}
					}
					m_sLastSavedProjData.mapFile2Size.insert(make_pair(uploadName, sz));
					m_sLastSavedProjData.strProjectID = strProjectID;

					uploadFiles(strProjectID, strContractId, *pFile, this);
				}
			}
		}
	}
}

void CGroupChatPage::ClearContractPage()
{
	// 合约名称
	CControlUI* pContractName = (CControlUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl(EDIT_CONTRACT_PAGE_NAME));
	if (NULL != pContractName)
	{
		pContractName->SetText(_T(""));
	}
	// 收款方/付款方
	COptionUI* OptionIn = static_cast<COptionUI*>(m_pPaintManager->FindControl(RADIO_CONTRACT_PAGE_IN));
	if(NULL != OptionIn)
	{
		OptionIn->Selected(true);
	}

	// 合约金额
	CControlUI* pContractMoney = (CControlUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl(EDIT_CONTRACT_PAGE_MONEY));
	if (NULL != pContractMoney)
	{
		pContractMoney->SetText(_T(""));
	}

	// 付款方式

	COptionUI* OptionAgree = static_cast<COptionUI*>(m_pPaintManager->FindControl(RADIO_CONTRACT_PAGE_AGREE));
	if(NULL != OptionAgree)
	{
		OptionAgree->Selected(true);
	}
	// 备注
	CControlUI* pControlMemo = (CControlUI*)static_cast<CControlUI*>(m_pPaintManager->FindControl(REDIT_CONTRACT_PAGE_INFO));
	tstring strMemo;
	if (NULL != pControlMemo)
	{
		pControlMemo->SetText(_T(""));
	}
	m_pContractFiles->RemoveAll();
}

void CGroupChatPage::OnGetGroupContactProperty()
{
	tstring strUrl("124.206.140.155:8090/commonApi/cssapi.do?txid=nla003");
	//	strUrl += _T("sign-up");

	orgInfo info;
	CWebClient::GetOrgByPhone(mainDlg::getInstance()->getMyUserInfo().strAccount, info);

	tstring strPost;
	std::ostringstream os;
	os<<"{";
	os<<"\"appcode\":\""<<"MMT"<<"\","
		<<"\"userid\":\""<<mainDlg::getInstance()->getMyUserInfo().strAccount<<"\","
		<<"\"username\":\""<<mainDlg::getInstance()->getMyUserInfo().strUserNickname<<"\","
		<<"\"orgid\":\""<<info.strId<<"\","
		<<"\"orgname\":\""<<info.strName<<"\","
		<<"\"mmbno\":\""<<info.strMemberId<<"\","//机构会员号		
		<<"\"rmmbno\":\""<<"321000"<<"\","//收方会员号
		<<"\"pmmbno\":\""<<"123000"<<"\""//付方会员号
		<<"}";

	strPost = os.str();

	tstring strUTF8Url;
	CChineseCode::GB2312ToUTF_8(strUTF8Url, strUrl.c_str(), strUrl.length());

	tstring strUTF8Post;
	CChineseCode::GB2312ToUTF_8(strUTF8Post, strPost.c_str(), strPost.length());

	tstring strHttpResponse;
	CHttpClient httpClient;
	int retCode = httpClient.CSSPost(strUrl, strPost, strHttpResponse);

	strHttpResponse = CChineseCode::DecodeUTF8(strHttpResponse);

	ReplaceAll(strHttpResponse, "\\", "");


	strHttpResponse = strHttpResponse.substr(1, strHttpResponse.length());
	strHttpResponse = strHttpResponse.substr(0, strHttpResponse.length()-1);

	if (strHttpResponse.find("retcode") != -1 && strHttpResponse.find("retmsg") != -1 &&
		strHttpResponse.find("ispay") != -1 && strHttpResponse.find("period") != -1)
	{
		tstring ret_code;
		tstring ret_string;
		tstring is_pay;
		int period;

		Json::Value root;
		Json::Reader reader;
		try
		{
			if (reader.parse(strHttpResponse,root))
			{
				ret_code =  root["retcode"].asString();
				ret_string =  root["retmsg"].asString();
				is_pay = root["ispay"].asString();
				period = root["period"].asInt();
			}
		}
		catch (...)
		{
			tstring str;
			str = "查询长协失败,Http应答:  " + strHttpResponse;
			CLogger::GetInstance()->PrintErrLog(str.c_str());		
		}

		if ("0" == ret_code)
		{
			//处理是否隐藏担保交易
			COptionUI* OptionNone = static_cast<COptionUI*>(m_pPaintManager->FindControl(RADIO_CONTRACT_PAGE_NONE));//pos="94,154,0,0"
			COptionUI* OptionSure = static_cast<COptionUI*>(m_pPaintManager->FindControl(RADIO_CONTRACT_PAGE_SURE));//pos="94,154,0,0"
			COptionUI* OptionAgree = static_cast<COptionUI*>(m_pPaintManager->FindControl(RADIO_CONTRACT_PAGE_AGREE));//pos="95,174,0,0"
			COptionUI* OptionLogistics = static_cast<COptionUI*>(m_pPaintManager->FindControl(RADOI_CONTRACT_PAGE_LOGISTICS));//pos="95,194,0,0"
			if (is_pay == "1")//这里的判断要验证，
			{				
				if(NULL != OptionSure)
				{
					OptionSure->SetInternVisible();
				}
				RECT rc;//(94, 174, 189, 170);
				rc.left = 94;
				rc.right = 189;
				rc.top = 154;
				rc.bottom = 170;
				OptionAgree->SetPos(rc);

				rc.top = 174;
				rc.bottom = 190;
				OptionLogistics->SetPos(rc);

				//担保交易
				OptionNone->Selected(false);
				OptionSure->Selected(true);
				OptionAgree->Selected(false);
				OptionLogistics->Selected(false);
			}
			else
			{
				if(NULL != OptionSure)
				{
					OptionSure->SetInternVisible(false);
				}

				//移动位置
				RECT rc;//(94, 174, 189, 170);
				rc.left = 94;
				rc.right = 189;
				rc.top = 134;
				rc.bottom = 150;
				OptionAgree->SetPos(rc);

				rc.top = 154;
				rc.bottom = 170;
				OptionLogistics->SetPos(rc);

				//周期结
				OptionNone->Selected(false);
				OptionSure->Selected(false);
				OptionAgree->Selected(true);
				OptionLogistics->Selected(false);

				//
				tstring _period = _T("协议支付");
				switch (period)
				{
				case 1:
					_period += _T("(现结)");
					break;
				case 2:
					_period += _T("(周结)");
					break;
				case 3:
					_period += _T("(月结)");
					break;
				default:
					break;
				}
				OptionAgree->SetName(_period.c_str());
			}
		}
		else
		{
			tstring str;
			str = "查询长协错误，错误代码：" + ret_code;
			CMsgBox* pMsgBox = new CMsgBox("错误", str, MSGBOX_ICON_ERROR);
			pMsgBox->ShowModal();

			return;
		}
	}
	else
	{
		CMsgBox* pMsgBox = new CMsgBox("错误","发送网络查询请求失败，请稍候重试！", MSGBOX_ICON_ERROR);
		pMsgBox->ShowModal();
	}	
}

void CGroupChatPage::OnSendContactMsgToCss()
{
	return;
	tstring strUrl("124.206.140.155:8090/commonApi/cssapi.do?txid=nar001");
	//	strUrl += _T("sign-up");

	orgInfo info;
	CWebClient::GetOrgByPhone(mainDlg::getInstance()->getMyUserInfo().strAccount, info);

	tstring strPost;
	std::ostringstream os;
	os<<"{";
	os<<"\"appcode\":\""<<"MMT"<<"\","
		<<"\"userid\":\""<<mainDlg::getInstance()->getMyUserInfo().strAccount<<"\","
		<<"\"username\":\""<<mainDlg::getInstance()->getMyUserInfo().strUserNickname<<"\","
		<<"\"orgid\":\""<<info.strId<<"\","
		<<"\"orgname\":\""<<info.strName<<"\","
		<<"\"mmbno\":\""<<info.strMemberId<<"\","//收方，机构会员号
		<<"\"pmmbno\":\""<<"123000"<<"\","
		<<"\"porgname\":\""<<"321000"<<"\","
		<<"\"atype\":\""<<"1"<<"\","
		<<"\"amount\":\""<<"100"<<"\","
		<<"\"note\":\""<<"测试"<<"\","
		<<"\"orderno\":\""<<"000000"<<"\""
		<<"}";

	strPost = os.str();

	tstring strUTF8Url;
	CChineseCode::GB2312ToUTF_8(strUTF8Url, strUrl.c_str(), strUrl.length());

	tstring strUTF8Post;
	CChineseCode::GB2312ToUTF_8(strUTF8Post, strPost.c_str(), strPost.length());

	tstring strHttpResponse;
	CHttpClient httpClient;
	int retCode = httpClient.Post(strUTF8Url, strUTF8Post, strHttpResponse);
}
