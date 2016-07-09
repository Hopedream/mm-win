#include "FrameBusinessData.h"
#include "mainDlg.h"

#include "webClient.h"
#include "../json/json.h"
#include "Utility.h"
#include "CDialogBuilderCallbackEx.h"
#include "FileHelper.h"
#include <ShellAPI.h>
#include "MsgBox.h"
#include "StringHelper.h"
#include <sstream>
#include "Controller.h"
#include "MUCRoomMgr.h"
#include "AddNewGroupMember.h"


CFrameBusinessData* CFrameBusinessData::m_pInstance = NULL;
CFrameBusinessData::CFrameBusinessData(void)
{
}


CFrameBusinessData::~CFrameBusinessData(void)
{
}

CFrameBusinessData* CFrameBusinessData::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CFrameBusinessData;
		m_pInstance->CreateWnd(mainDlg::getInstance()->GetHWND());
	}
	return m_pInstance;
}

void CFrameBusinessData::HandleSetFocusEvent( TNotifyUI& msg )
{

}

void CFrameBusinessData::HandleKillFocusEvent( TNotifyUI& msg )
{

}

void CFrameBusinessData::HandleTextChangedEvent( TNotifyUI& msg )
{

}

void CFrameBusinessData::HandleItemClickEvent( TNotifyUI& msg )
{
	if (0 == msg.pSender->GetName().Find("BussinessDataListItem_")
		&& (NULL !=  m_pLstFavorite)
		&& m_pLstFavorite->GetItemIndex(msg.pSender) != -1)
	{
		if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
		{
			Node* node = (Node*)msg.pSender->GetTag();
			if (m_pLstFavorite->CanExpand(node))
			{
				m_pLstFavorite->SetChildVisible(node, !node->data().child_visible_);
			}
		}
	}
	else if (0 == msg.pSender->GetName().Find("BussinessDataListItem_")
		&& (NULL !=  m_pLstTrans)
		&& m_pLstTrans->GetItemIndex(msg.pSender) != -1)
	{
		if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
		{
			Node* node = (Node*)msg.pSender->GetTag();
			if (m_pLstTrans->CanExpand(node))
			{
				m_pLstTrans->SetChildVisible(node, !node->data().child_visible_);
			}
		}
	}
}

void CFrameBusinessData::HandleClickEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("buttonClose"))== 0)
	{
		ShowWindow(false);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("Btn_open"))== 0)
	{
		m_vecFiles.clear();
		fileInfo *pInfo = (fileInfo*)msg.pSender->GetParent()->GetParent()->GetTag();
		tstring strFilePath = CFileHelper::GetPackDir(pInfo->strPackName, pInfo->strPackID);
		strFilePath += pInfo->strFileFullName;//

		////先判断有没有这个文件了，如有就直接更新状态为 ”打开“可用
		bool bExist = PathFileExists(strFilePath.c_str());
		if (bExist)
		{
			ShellExecute(NULL, "open", strFilePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
		else
		{
			//如果没有，先下载
			CButtonUI *pBtn = (CButtonUI *)msg.pSender;
			pBtn->SetText("下载中");
			pBtn->SetEnabled(false);
			pInfo->strFilePath = strFilePath;
			pInfo->strKey = strFilePath;
			

			m_mapBtnOpen.insert(make_pair(strFilePath, pBtn));
			m_vecFiles.push_back(*pInfo);
			bool bRes = m_FileControl.downloadFiles(m_vecFiles[0], (IOssEventBase*)this);
		}
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("Button_open"))== 0)
	{
		m_vecFiles.clear();
		Node *pInfo = (Node*)msg.pSender->GetParent()->GetParent()->GetTag();
		tstring strFilePath = CFileHelper::GetPackDir(pInfo->data().strPackName, pInfo->data().strPackID);
		strFilePath += pInfo->data().strFileName;//

		////先判断有没有这个文件了，如有就直接更新状态为 ”打开“可用
		bool bExist = PathFileExists(strFilePath.c_str());
		if (bExist)
		{
			ShellExecute(NULL, "open", strFilePath.c_str(), NULL, NULL, SW_SHOWNORMAL);
		}
		else
		{
			//如果没有，先下载
			CButtonUI *pBtn = (CButtonUI *)msg.pSender;
			pBtn->SetText("下载中");
			pBtn->SetEnabled(false);
			fileInfo info;
			info.strFileFullName = pInfo->data().strFileName;
			info.strFilePath = strFilePath;
			info.strKey = strFilePath;
			info.strFileUrl = pInfo->data().strAliUrl;
			info.dwFileSize = atol(pInfo->data().strFileSize.c_str());
			
			m_mapBtnOpen.insert(make_pair(strFilePath, pBtn));
			m_vecFiles.push_back(info);
			bool bRes = m_FileControl.downloadFiles(m_vecFiles[0], (IOssEventBase*)this);


			//CMsgBox* pMsgBox = new CMsgBox("打开", "打开文件失败",  MSGBOX_ICON_WARNING);
			//pMsgBox->ShowModal(this->GetHWND());
		}
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("Button_send"))== 0)
	{
		Node *pInfo = (Node*)msg.pSender->GetParent()->GetParent()->GetTag();
		tstring strRoomID = m_pProItem->strRoomID;

		ostringstream os;
		CFileHelper helper;
		os<<"#COMMAND#"<<"type="<<helper.GetDescription(enmCmd_GroupPackFile).c_str()<<","
			<<"{"
			<<"\"createBy\":\""<<pInfo->data().strPackCreaterPhone/*strPackSender*/<<"\","
			<<"\"packid\":\""<<pInfo->data().strPackID<<"\","
			<<"\"attribute\":\""<<GetAttibuteValue(pInfo->data().strPackProp)<<"\","
			<<"\"packname\":\""<<pInfo->data().strPackName<<"\","
			<<"\"aliNormalFileList\":[";

		int nFileCount = pInfo->data().m_vecFiles.size();
		int nIdx=0;
		for (int i=0; i<nFileCount; i++)
		{
			os<<"{"
				<<"\"file_name\":\""<<pInfo->data().m_vecFiles[i].strFileFullName<<"\","
				<<"\"file_type\":\""<<pInfo->data().m_vecFiles[i].strFileType<<"\","
				<<"\"file_size\":\""<<pInfo->data().m_vecFiles[i].strFileSz<<"\","
				<<"\"ali_file_url\":\""<<pInfo->data().m_vecFiles[i].strFileUrl<<"\""
				<<"}";

			nIdx++;
			if (nIdx <nFileCount)
			{
				os<<",";
			}
		}

		os<<"]}";

		CController::Instance()->GetRoomManager()->SendMessage(strRoomID, os.str());


		//if (NULL != pData)
		//{
			//没有创建房间，就需要重新选人。
			if (strRoomID.empty())
			{

				CAddNewGroupMember* pAddGroupMemWnd = new CAddNewGroupMember(&m_PaintManager);
				if (NULL != pAddGroupMemWnd)
				{
					pAddGroupMemWnd->SetShowMode(true);
					pAddGroupMemWnd->CreateWnd();
					pAddGroupMemWnd->Initialize();//初始化一次。
				}

				pAddGroupMemWnd->SetResProperty(false, m_pProItem->strProjID, m_pProItem->strProjName);
				pAddGroupMemWnd->ShowWindow(true);
			}
			else
			{
				//我有可能不是这个群里面的成员，就需要判断下。如果我不是这个群里面的人，就不能进去。
				tstring strMyPhone = CController::Instance()->GetXmppClient()->jid().username();
				tstring strAccount = strMyPhone + MMPC_ACCOUNT;
				if(CController::Instance()->GetRoomManager()->IsInThisRoom(m_pProItem->strRoomID, strAccount))
				{
					CController::Instance()->GetRoomManager()->EnterRoom(m_pProItem->strRoomID, strMyPhone);
					if (m_pProItem->strDestOrgID.empty() && m_pProItem->strDestOrgName.empty())
					{
						if (m_pProItem->vecDestParticipants.size()>0)
						{
							mainDlg::getInstance()->OnShowGroupChatWnd_Personal(m_pProItem->vecDestParticipants[0], m_pProItem->strRoomID);
						}
					}
					else
					{
						mainDlg::getInstance()->OnShowGroupChatWnd(m_pProItem->strDestOrgID, m_pProItem->strDestOrgName, m_pProItem->strRoomID);
					}
				}
				else
				{
					CMsgBox* pMsgBox = new CMsgBox("错误","您不是该群组成员！");
					pMsgBox->ShowModal();
				}
			}
			ShowWindow(false);
	}

	else if (_tcsicmp(msg.pSender->GetName(), _T("Button_delete"))== 0)
	{
		Node *pInfo = (Node*)msg.pSender->GetParent()->GetParent()->GetTag();
		tstring strStatus("0");
		CWebClient::updatePackStatus(pInfo->data().strPackID, strStatus);
		updateWindow();
	}
}

void CFrameBusinessData::HandleSelectedChangedEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("Option_Contract")) ==0 )
	{
		m_pTabLayout->SelectItem(0);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("Option_Favorite")) ==0 )
	{
		m_pTabLayout->SelectItem(1);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("Option_Trans")) ==0 )
	{
		m_pTabLayout->SelectItem(2);
	}
}

void CFrameBusinessData::HandleItemSelectEvent( TNotifyUI& msg )
{

}

DuiLib::CDuiString CFrameBusinessData::GetSkinFile()
{
	return _T("Bussiness_data_view.xml");
}

CControlUI* CFrameBusinessData::CreateControl( LPCTSTR pstrClass )
{
	if (_tcsicmp(pstrClass, _T("BussinessDataList")) == 0)
	{
		return new CUIBussinessDataList(m_PaintManager);
	}
}

void CFrameBusinessData::InitWindow()
{
	m_pTabLayout = static_cast<CTabLayoutUI*>(m_PaintManager.FindControl("TabLayoutUI1"));
	initTabs();
}

void CFrameBusinessData::initTabs()
{
	if (m_pTabLayout == NULL)
		return;
	// 0 签约资料
	CDialogBuilder  m_DlgBuilder0;
	
	CVerticalLayoutUI *ptabContact = NULL;
	if (!m_DlgBuilder0.GetMarkup()->IsValid())
	{
		ptabContact = static_cast<CVerticalLayoutUI*>(m_DlgBuilder0.Create(_T("Bussiness_data_tab_contract.xml"),
			(UINT)0, NULL, &m_PaintManager));
	}
	else
	{
		ptabContact = static_cast<CVerticalLayoutUI*>(m_DlgBuilder0.Create((UINT)0, &m_PaintManager));
	}
	if (ptabContact != NULL)
	{
		m_pTabLayout->AddAt(ptabContact, 0);
	}
	else
		return;


	// 1 收藏资料
	CDialogBuilder  m_DlgBuilder1;
	CDialogBuilderCallbackEx cb1(&m_PaintManager);
	CVerticalLayoutUI *ptabFavorite = NULL;
	if (!m_DlgBuilder1.GetMarkup()->IsValid())
	{
		ptabFavorite = static_cast<CVerticalLayoutUI*>(m_DlgBuilder1.Create(_T("Bussiness_data_tab_favorite.xml"),
			(UINT)0, &cb1, &m_PaintManager));
	}
	else
	{
		ptabFavorite = static_cast<CVerticalLayoutUI*>(m_DlgBuilder1.Create((UINT)0, &m_PaintManager));
	}

	if (ptabFavorite != NULL)
	{
		m_pTabLayout->AddAt(ptabFavorite, 1);
	}
	else
		return;

	//2 收发资料
	CDialogBuilder  m_DlgBuilder2;
	CDialogBuilderCallbackEx cb2(&m_PaintManager);
	CVerticalLayoutUI *ptabTrans = NULL;
	if (!m_DlgBuilder2.GetMarkup()->IsValid())
	{
		ptabTrans = static_cast<CVerticalLayoutUI*>(m_DlgBuilder2.Create(_T("Bussiness_data_tab_trans.xml"),
			(UINT)0, &cb2, &m_PaintManager));
	}
	else
	{
		ptabTrans = static_cast<CVerticalLayoutUI*>(m_DlgBuilder2.Create((UINT)0, &m_PaintManager));
	}
	if (ptabTrans != NULL)
	{
		m_pTabLayout->AddAt(ptabTrans, 2);
	}
	else
		return;

	//追加到TabLayout Container容器内，索引值+1
	m_pTabLayout->SelectItem(0);

	m_pLstContract = static_cast<CListUI*>(m_PaintManager.FindSubControlByName(ptabContact,"ListUI1"));
	m_pLstFavorite = static_cast<CUIBussinessDataList*>(m_PaintManager.FindSubControlByName(ptabFavorite, "ListFav"));
	m_pLstTrans = static_cast<CUIBussinessDataList*>(m_PaintManager.FindSubControlByName(ptabTrans,"ListTrans"));
}

void CFrameBusinessData::initContract()
{
	m_vcContractItems.clear();
	m_pLstContract->RemoveAll();
	tstring strStatus("3");
	tstring strRes = CWebClient::api_getPacks(m_pProItem->strProjID, strStatus);
	//strRes = CChineseCode::DecodeUTF8(strRes);

	if (strRes.find("name") != -1
		&& strRes.find("attribute") != -1
		&& strRes.find("create_by") != -1
		&& strRes.find("create_time") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		try
		{
			if (reader.parse(strRes, root))
			{
				int iSize = root.size();
				for (int i=0; i<iSize; i++)
				{
					sProjectInfo oneInfo;
					oneInfo.m_vcFiles.clear();
					oneInfo.strProjectName = root[i]["name"].asString();
					oneInfo.strProperty = root[i]["attribute"].asString();
					oneInfo.strRecipient = root[i]["desc"].asString();
					oneInfo.strSender = root[i]["create_by"].asString();
					//sFriendListItem friendinfo;
					//mainDlg::getInstance()->GetFriendItem(oneInfo.strSender, friendinfo);
					//oneInfo.strSender = friendinfo.strShowName;
					m_strOrgName = root[i]["org_name"].asString();
					oneInfo.strTime = root[i]["create_time"].asString();
					int iFilesize = root[i]["file_lists"].size();
					for (int j=0; j<iFilesize; j++)
					{
						fileInfo onefileInfo;
						onefileInfo.strFileFullName = root[i]["file_lists"][j]["file_name"].asString();
						onefileInfo.strFileType = root[i]["file_lists"][j]["file_type"].asString();
						tstring strFilesize = root[i]["file_lists"][j]["file_size"].asString();
						onefileInfo.strFileUrl = root[i]["file_lists"][j]["ali_file_url"].asString();
						onefileInfo.strPackID = root[i]["id"].asString();
						onefileInfo.strPackName = oneInfo.strProjectName;
						onefileInfo.dwFileSize = atol(strFilesize.c_str());
						oneInfo.m_vcFiles.push_back(onefileInfo);
					}
					m_vcContractItems.push_back(oneInfo);
					int af = 0;
				}
			}
		}
		catch (...)
		{

		}	
	}
	else if (strRes == "[]")
	{
		return;
	}
	else
	{
		CMsgBox* pMsgBox = new CMsgBox("错误","获取Pack失败！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal(NULL);
		return;
	}
	
	if (m_vcContractItems.size()>0)
	{
		//UI
		for (int i=0; i<m_vcContractItems.size(); i++)
		{
			addContractItem(m_vcContractItems[i]);
		}
	}
}

void CFrameBusinessData::initFavorite()
{
	if (m_pLstFavorite == NULL)
		return;
	m_pLstFavorite->RemoveAll();
	tstring strStatus("1");   //todo
	tstring strRes = CWebClient::api_getPacks(m_pProItem->strProjID, strStatus);
	//strRes = CChineseCode::DecodeUTF8(strRes);

	if (strRes.find("name") != -1 && strRes.find("attribute") != -1 && strRes.find("create_by") != -1 
		&& strRes.find("create_time") != -1 && strRes.find("org_name") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		try
		{
			if (reader.parse(strRes, root))
			{
				int iSize = root.size();
				for (int i=0; i<iSize; i++)
				{
					BDLstItemInfo oneInfo;
					oneInfo.strProName = root[i]["name"].asString();
					oneInfo.strAttribute = root[i]["attribute"].asString();
					oneInfo.strCollect = root[i]["create_by"].asString();
					oneInfo.strTime = root[i]["create_time"].asString();
					oneInfo.strPackSender = root[i]["cell_phone"].asString();
					oneInfo.isFavorite = true;
					oneInfo.strPackId = root[i]["id"].asString();
					m_strOrgName = root[i]["org_name"].asString();
					oneInfo.folder = true;
					oneInfo.itemId = GetNextItemID();
					oneInfo.m_vecFiles.clear();
					int iFilesize = root[i]["file_lists"].size();
					for (int j=0; j<iFilesize; j++)
					{
						fileInfo file;
						file.strFileFullName = root[i]["file_lists"][j]["file_name"].asString();
						file.strFileType = root[i]["file_lists"][j]["file_type"].asString();
						file.strFileSz = root[i]["file_lists"][j]["file_size"].asString();
						file.strFileUrl = root[i]["file_lists"][j]["ali_file_url"].asString();
						oneInfo.m_vecFiles.push_back(file);
					}
					Node* pParentNode = m_pLstFavorite->AddNode(oneInfo, NULL);


					for (int j=0; j<iFilesize; j++)
					{
						BDLstItemInfo oneInfo_child;
						oneInfo_child.folder = false;
						oneInfo_child.itemId = GetNextItemID();

						oneInfo_child.strFileFullName = root[i]["file_lists"][j]["file_name"].asString();
						oneInfo_child.strFileType = root[i]["file_lists"][j]["file_type"].asString();
						oneInfo_child.strSize = root[i]["file_lists"][j]["file_size"].asString();
						oneInfo_child.strUrl = root[i]["file_lists"][j]["ali_file_url"].asString();
						//oneInfo_child.strPackId = root[i]["id"].asString();
						oneInfo_child.strPackName = oneInfo.strProName;
						m_pLstFavorite->AddNode(oneInfo_child, pParentNode);
					}
				}
			}
		}
		catch (...)
		{

		}	
	}
	else if (strRes == "[]")
	{
		return;
	}
	else
	{
		CMsgBox* pMsgBox = new CMsgBox("错误", "获取文件列表失败！！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal(NULL);
	}
	
}

void CFrameBusinessData::initTrans()
{
	if (m_pLstTrans == NULL)
		return;
	m_pLstTrans->RemoveAll();
	tstring strStatus("2"); // todo
	tstring strRes = CWebClient::api_getPacks(m_pProItem->strProjID, strStatus);
	//strRes = CChineseCode::DecodeUTF8(strRes);
	if (!strRes.empty() && strRes.find("name") != -1 && strRes.find("attribute") != -1 
		&& strRes.find("create_by") != -1 && strRes.find("cell_phone") != -1 && strRes.find("org_name") != -1 )
	{
		Json::Reader reader;
		Json::Value root;
		try
		{
			if (reader.parse(strRes, root))
			{
				int iSize = root.size();
				for (int i=0; i<iSize; i++)
				{
					BDLstItemInfo oneInfo;
					oneInfo.isFavorite = false;
					oneInfo.strProName = root[i]["name"].asString(1);
					oneInfo.strAttribute = root[i]["attribute"].asString(1);
					oneInfo.strCollect = root[i]["create_by"].asString(1);
					oneInfo.strPackSender = root[i]["cell_phone"].asString(1);
					oneInfo.strTime = root[i]["create_time"].asString(1);
					m_strOrgName = root[i]["org_name"].asString(1);
					oneInfo.strPackId = root[i]["id"].asString(1);
					int iStatus = root[i]["status"].asInt();
					switch (iStatus)
					{
					case 0:
						oneInfo.strStatus = _T("默认");
						break;
					case 1:
						oneInfo.strStatus = _T("收藏");
						break;
					case 2:
						oneInfo.strStatus = _T("收发");
						break;
					case 3:
						oneInfo.strStatus = _T("同意");
						break;
					case 4:
						oneInfo.strStatus = _T("拒绝");
						break;
					default:
						break;
					}
					oneInfo.folder = true;
					oneInfo.itemId = GetNextItemID();
					int iFilesize = root[i]["file_lists"].size();
					for (int j=0; j<iFilesize; j++)
					{
						fileInfo file;
						file.strFileFullName = root[i]["file_lists"][j]["file_name"].asString(1);
						file.strFileType = root[i]["file_lists"][j]["file_type"].asString(1);
						file.strFileSz = root[i]["file_lists"][j]["file_size"].asString(1);
						file.strFileUrl = root[i]["file_lists"][j]["ali_file_url"].asString(1);
						oneInfo.m_vecFiles.push_back(file);
					}
					Node* pParentNode = m_pLstTrans->AddNode(oneInfo, NULL);


					for (int j=0; j<iFilesize; j++)
					{
						BDLstItemInfo oneInfo_child;
						oneInfo_child.folder = false;
						oneInfo_child.itemId = GetNextItemID();

						oneInfo_child.strFileFullName = root[i]["file_lists"][j]["file_name"].asString(1);
						oneInfo_child.strFileType = root[i]["file_lists"][j]["file_type"].asString(1);
						oneInfo_child.strSize = root[i]["file_lists"][j]["file_size"].asString(1);
						oneInfo_child.strUrl = root[i]["file_lists"][j]["ali_file_url"].asString(1);
						oneInfo_child.strPackId = root[i]["id"].asString(1);
						oneInfo_child.strPackName = oneInfo.strProName;
						m_pLstTrans->AddNode(oneInfo_child, pParentNode);
					}
				}
			}
		}
		catch (...)
		{

		}	
	}
	else if (strRes == "[]")
	{
		return;
	}
	else
	{
		CMsgBox* pMsgBox = new CMsgBox("错误", "获取文件列表失败：line577", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal(NULL);
	}

}

void CFrameBusinessData::setProjectItem( sProjectItem* item)
{
	m_pProItem = item;
	initContract();
	initFavorite();
	initTrans();
	//m_pTabLayout->SelectItem(0);
	CLabelUI *pLabel_Title = static_cast<CLabelUI*>(m_PaintManager.FindControl(_T("Label_Title")));
	tstring strTitle("(");
	strTitle += m_pProItem->strProjID + m_strOrgName + ")";
	pLabel_Title->SetText(strTitle.c_str());
}

void CFrameBusinessData::addContractItem( sProjectInfo& oneInfo)
{
	CDialogBuilder DlgBuilder;
	CListContainerElementUI *plstItem = NULL;
	if (!DlgBuilder.GetMarkup()->IsValid())
	{
		plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create(_T("bussiness_data_contract_item.xml"),
			(UINT)0, NULL, &m_PaintManager));
	}
	else
	{
		plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create((UINT)0, &m_PaintManager));
	}
	if (plstItem == NULL)
		return;
	else
	{
		CLabelUI *plabelName = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("Label_Name")));
		if (plabelName!= NULL)
			plabelName->SetText(oneInfo.strProjectName.c_str());
		CLabelUI *plablAttribute= static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("Label_Attribute")));
		if (plablAttribute != NULL)
		{
			plablAttribute->SetText(oneInfo.strProperty.c_str());
		}
		CLabelUI *plablCreate_time = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("Label_Create_time")));
		if (plablCreate_time != NULL)
		{
			plablCreate_time->SetText(oneInfo.strTime.c_str());
		}

		CLabelUI *plablOrg_name = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("Label_Org_name")));
		if (plablOrg_name != NULL)
		{
			plablOrg_name->SetText(oneInfo.strSender.c_str());
		}

	/*	CLabelUI *plablDesc = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("Label_Desc")));
		if (plablDesc != NULL)
		{
			plablDesc->SetText(oneInfo.strRecipient.c_str());
		}*/

		for (int i=0; i<oneInfo.m_vcFiles.size(); i++)
		{
			addContractChildItem(oneInfo.m_vcFiles[i], plstItem);
		}
		plstItem->SetFixedHeight(191);
		if (!m_pLstContract->Add(plstItem))
		{
			delete plstItem;
			return;
		}
	}
}

void CFrameBusinessData::addContractChildItem( fileInfo& oneInfo, CListContainerElementUI* pParent)
{

	CDialogBuilder DlgBuilder;
	CListContainerElementUI *plstItemChild = NULL;
	if (!DlgBuilder.GetMarkup()->IsValid())
	{
		plstItemChild = static_cast<CListContainerElementUI*>(DlgBuilder.Create(_T("bussiness_data_contract_item_item.xml"),
			(UINT)0, NULL, &m_PaintManager));
	}
	else
	{
		plstItemChild = static_cast<CListContainerElementUI*>(DlgBuilder.Create((UINT)0, &m_PaintManager));
	}
	if (plstItemChild == NULL)
		return;
	else
	{
		CLabelUI *pLabelFileName = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItemChild, _T("Label_fileName")));
		if (pLabelFileName != NULL)
		{
			pLabelFileName->SetText(oneInfo.strFileFullName.c_str());
		}
		CControlUI *pctrlFileLog = static_cast<CControlUI *>(m_PaintManager.FindSubControlByName(plstItemChild, _T("Control_fileLog")));
		if (pctrlFileLog != NULL)
		{
			
			pctrlFileLog->SetBkImage(GetPackFileIcon(oneInfo.strFileType).c_str());
		}

		CLabelUI *pLabelFileSize = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItemChild, _T("Label_fileSize")));
		if (pLabelFileSize != NULL)
		{
			pLabelFileSize->SetText(GetFileSizeFormat(oneInfo.dwFileSize).c_str());
		}
		CListUI* pLstContractChild = static_cast<CListUI*>(m_PaintManager.FindSubControlByName(pParent, _T("List_Files")));
		plstItemChild->SetFixedHeight(50);
		plstItemChild->SetTag((UINT_PTR)(&oneInfo));
		if (!pLstContractChild->Add(plstItemChild))
		{
			delete plstItemChild;
			return;
		}
	}
}

CUIBussinessDataList* CFrameBusinessData::getFavoritelst()
{
	return m_pLstFavorite;
}

void CFrameBusinessData::OnProcess( tstring& fileName, int iValue, tstring& strKey )
{
	if (iValue == 98 || iValue == -1)
	{
		CButtonUI *pBtn;
		map<tstring, CButtonUI*>::iterator iter = m_mapBtnOpen.find(fileName);
		if (iter == m_mapBtnOpen.end())
			return;
		pBtn = iter->second;
		if (pBtn == NULL)
			return;
		pBtn->SetEnabled(true);
		pBtn->SetText("打开");
		m_mapBtnOpen.erase(iter);
	}
}

void CFrameBusinessData::updateWindow()
{
	initContract();
	initFavorite();
	initTrans();
}


