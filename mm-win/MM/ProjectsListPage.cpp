#include "ProjectsListPage.h"
#include "CDialogBuilderCallbackEx.h"
#include "Utility.h"
#include <time.h>
#include "MsgBox.h"
#include "MUCRoomMgr.h"
#include "Controller.h"
#include "mainDlg.h"
#include "AddNewGroupMember.h"
#include "webClient.h"
#include "FrameBusinessData.h"
#include "Controller.h"
#include "UserManager.h"
#include "..\include\DBTaskModule.h"


const int const_total_page_btn_count = 10;
const int const_max_items_in_page = 8;
const int const_one_week_seconds  = 7*24*3600;
const int const_one_month_seconds = 30*24*3600;//��ʱ��ȡһ����ƽ��ֵ30����㡣
const int const_nButtonOffset	  = 33;
const int const_button_gap		  = 20;

CProjectInfoPage::CProjectInfoPage(DuiLib::CPaintManagerUI* ppm)
:m_pListProjs(NULL)
,m_pProjProcessor(NULL)
//,m_pAddGroupMemWnd(NULL)
,m_pBtnPrevPage(NULL)
,m_pBtnNextPage(NULL)
,m_pBtnEndPage(NULL)
,m_nCurrentPageIndex(-1)
,m_nEndPageIndex(-1)
{
	m_PaintManager = ppm;

	CDialogBuilder DlgBuilder;
	CVerticalLayoutUI *pProjectPage = NULL;
	CDialogBuilderCallbackEx cb(m_PaintManager);
	if (!DlgBuilder.GetMarkup()->IsValid())
	{
		pProjectPage = static_cast<CVerticalLayoutUI*>(DlgBuilder.Create(_T("tab_project_page.xml"),
			(UINT)0, &cb, m_PaintManager));
	}
	else
	{
		pProjectPage = static_cast<CVerticalLayoutUI*>(DlgBuilder.Create((UINT)0, m_PaintManager));
	}
	static_cast<CTabLayoutUI*>(ppm->FindControl("SidebarTabContainer"))->Add(pProjectPage);	

	m_PaintManager->AddNotifier(this);
}

CProjectInfoPage::~CProjectInfoPage()
{
	m_PaintManager->RemoveNotifier(this);
}

//��ȡ�û���Ϣ������ʼ��������
bool CProjectInfoPage::Initialize()
{
	m_pBtnFirstpage = static_cast<CButtonUI*>(m_PaintManager->FindControl("btn_page_first"));
	m_pBtnNextPage = static_cast<CButtonUI*>(m_PaintManager->FindControl("btn_page_next"));
	m_pBtnEndPage  = static_cast<CButtonUI*>(m_PaintManager->FindControl("btn_page_end"));
	m_pBtnPrevPage = static_cast<CButtonUI*>(m_PaintManager->FindControl("btn_page_prev"));

	//UpdatePageButton();

	//����ʾ4����ť��
	m_pBtnFirstpage->SetVisible(false);
	m_pBtnPrevPage->SetVisible(false);
	m_pBtnNextPage->SetVisible(false);
	m_pBtnEndPage->SetVisible(false);

	//��ʼ��1--10(���)��
	for (int i=1; i<=10; i++)
	{
		char buf[100]={0};
		sprintf_s(buf, sizeof(buf), _T("btn_page_%d"), i);
		CControlUI* pControlPagebtn= (CControlUI*)static_cast<CControlUI*>(m_PaintManager->FindControl(buf));
		if (NULL != pControlPagebtn)
		{
			sprintf_s(buf, sizeof(buf), _T("%d"), i);
			pControlPagebtn->SetAttribute("textcolor", "#ff000000");
			pControlPagebtn->SetText(buf);
			pControlPagebtn->SetVisible(false);
		}
	}

	return true;
}

void CProjectInfoPage::HandleWindowInitEvent( TNotifyUI& msg )
{
	m_pListProjs = static_cast<CListUI*>(m_PaintManager->FindControl(_T("listProjs")));

	//������Ŀ�߼�ģ��
	if (NULL == m_pProjProcessor)
	{
		m_pProjProcessor = new CProjectProcessor;
	}
	
	//tstring strStartData = GetYMDTimeString(time(0));
	//FetchProjects(strStartData);
}

void CProjectInfoPage::RefreshPage()
{
	tstring strStartData = GetYMDTimeString(time(0));
	FetchProjects(strStartData);
}

void CProjectInfoPage::Notify( TNotifyUI& msg )
{
	if (0 == _tcsicmp(DUI_MSGTYPE_WINDOWINIT, msg.sType))
	{
		HandleWindowInitEvent(msg);
	}

	if (msg.sType != DUI_MSGTYPE_CLICK )
	{
		return;
	}

	DuiLib::CDuiString strName = msg.pSender->GetName();

	//"��һҳ"
	if (_tcsicmp(strName.GetData(), "btn_page_first") == 0)
	{
		if (!IsCacheEmpty()&& !IsFirstPage())
		{
			GotoPage(1);
		}	
	}
	//"��һҳ"
	else if (_tcsicmp(strName.GetData(), "btn_page_prev") == 0)
	{
		if (!IsCacheEmpty() && !IsFirstPage())
		{
			if (m_nCurrentPageIndex>1)
			{
				GotoPage(m_nCurrentPageIndex-1);
			}
		}
	}
	//"��һҳ"
	else if (_tcsicmp(strName.GetData(), "btn_page_next") == 0)
	{
		if (!IsCacheEmpty() && !IsEndPage())
		{
			if (m_nCurrentPageIndex<m_nEndPageIndex)
			{
				GotoPage(m_nCurrentPageIndex+1);
			}
		}
	}
	//"���һҳ"
	else if (_tcsicmp(strName.GetData(), "btn_page_end") == 0)
	{
		if (!IsCacheEmpty()&& !IsEndPage())
		{
			if (m_nEndPageIndex>0)
			{
				GotoPage(m_nEndPageIndex);
			}	
		}
	}
	//1--10ҳ��ť
	else if (strName.Find("btn_page_")>=0)
	{
		//tstring strSearch = "btn_page_";
		//tstring strBtnIndex = strName.Right(strName.GetLength()-strSearch.length());
		//int nIndex = atoi(strBtnIndex.c_str());

		int nIndex = atoi(msg.pSender->GetText());
		GotoPage(nIndex);
		return;
	}

	if (_tcsicmp(strName.GetData(), "OptionToday") == 0)
	{
		time_t tToday = time(0);
		FetchProjects(GetYMDTimeString(tToday));
	}
	else if (_tcsicmp(strName.GetData(), "OptionWeek") == 0)
	{
		time_t tToday = time(0);
		time_t tOneWeekAgo = tToday - const_one_week_seconds;
		FetchProjects(GetYMDTimeString(tOneWeekAgo));
	}
	else if (_tcsicmp(strName.GetData(), "OptionMonth") == 0)
	{
		time_t tToday = time(0);
		time_t tOneMonthAgo = tToday - const_one_month_seconds;
		FetchProjects(GetYMDTimeString(tOneMonthAgo));
	}
	else if (_tcsicmp(strName.GetData(), "OptionAll") == 0)
	{
		FetchProjects(tstring(""));
	}
	else if (msg.pSender->GetName() == "btn_get_project_info")
	{
		sProjectItem* pData = (sProjectItem*)msg.pSender->GetParent()->GetParent()->GetParent()->GetTag();
		if (NULL != pData)
		{
			CFrameBusinessData::Instance()->ShowWindow(true);
			CFrameBusinessData::Instance()->setProjectItem(pData);
		}
	}
	else if (msg.pSender->GetName() == "btn_bussiness_chat")
	{
		sProjectItem* pData = (sProjectItem*)msg.pSender->GetParent()->GetParent()->GetParent()->GetTag();
		if (NULL != pData)
		{
			//û�д������䣬����Ҫ����ѡ�ˡ�
			if (pData->strRoomID.empty())
			{
				
				CAddNewGroupMember* pAddGroupMemWnd = new CAddNewGroupMember(m_PaintManager);
				if (NULL != pAddGroupMemWnd)
				{
					pAddGroupMemWnd->SetShowMode(true);
					pAddGroupMemWnd->CreateWnd();
					pAddGroupMemWnd->Initialize();//��ʼ��һ�Ρ�
				}
				
				//bool bWithProperty = m_pComboResProperty->GetCurSel() == 1;
				pAddGroupMemWnd->SetResProperty(false, pData->strProjID, pData->strProjName);
				pAddGroupMemWnd->ShowWindow(true);
			}
			else
			{
				//���п��ܲ������Ⱥ����ĳ�Ա������Ҫ�ж��¡�����Ҳ������Ⱥ������ˣ��Ͳ��ܽ�ȥ��
				tstring strMyPhone = CController::Instance()->GetXmppClient()->jid().username();
				tstring strMyAccount = strMyPhone+MMPC_ACCOUNT;
				if(CController::Instance()->GetRoomManager()->IsInThisRoom(pData->strRoomID, strMyAccount ))
				{
					CController::Instance()->GetRoomManager()->EnterRoom(pData->strRoomID, strMyPhone);
					if (pData->strDestOrgID.empty() && pData->strDestOrgName.empty())
					{
						if (pData->vecDestParticipants.size()>0)
						{
							mainDlg::getInstance()->OnShowGroupChatWnd_Personal(pData->vecDestParticipants[0], pData->strRoomID);
						}
					}
					else
					{
						mainDlg::getInstance()->OnShowGroupChatWnd(pData->strDestOrgID, pData->strDestOrgName, pData->strRoomID);
					}
				}
				else
				{
					CMsgBox* pMsgBox = new CMsgBox("����","�����Ǹ�Ⱥ���Ա��");
					pMsgBox->ShowModal();
				}
			}
			
		}		
	}	
	//������������ by HQ 20160519
	else if (msg.pSender->GetName() == "btn_Search")
	{	
		COptionUI* OptionToday = static_cast<COptionUI*>(m_PaintManager->FindControl("OptionToday"));
		COptionUI* OptionWeek = static_cast<COptionUI*>(m_PaintManager->FindControl("OptionWeek"));
		COptionUI* OptionMonth = static_cast<COptionUI*>(m_PaintManager->FindControl("OptionMonth"));
		COptionUI* OptionAll = static_cast<COptionUI*>(m_PaintManager->FindControl("OptionAll"));

		tstring sSelectedTime = "";
		if (OptionToday->IsSelected())
		{
			time_t tToday = time(0);
			sSelectedTime = GetYMDTimeString(tToday);
		}
		else if (OptionWeek->IsSelected())
		{
			time_t tToday = time(0);
			time_t tOneWeekAgo = tToday - const_one_week_seconds;
			sSelectedTime = GetYMDTimeString(tOneWeekAgo);
		}
		else if (OptionMonth->IsSelected())
		{
			time_t tToday = time(0);
			time_t tOneMonthAgo = tToday - const_one_month_seconds;
			sSelectedTime = GetYMDTimeString(tOneMonthAgo);
		}
		else if (OptionAll->IsSelected())
		{
			//do nothing...
		}

		CEditUI* edtSearchString = static_cast<CEditUI*>(m_PaintManager->FindControl("edt_SearchString"));
		tstring sSearchString = edtSearchString->GetText();

		//������ַ�����ѯ by HQ 20160519
		//if(sSearchString == "")
		//{
		//	CMsgBox* pMsgBox = new CMsgBox("����","���������������");
		//	pMsgBox->ShowModal();
		//	edtSearchString->SetFocus();
		//}
		//else
		//{
			FetchProjects(sSelectedTime,sSearchString);
		//}
	}	
}

void CProjectInfoPage::FetchProjects(tstring& strStartDataTime,const tstring& strSearch)
{
	//1:Remove all first.
	RemoveAllListItems();


	m_pBtnFirstpage->SetVisible(false);
	m_pBtnPrevPage->SetVisible(false);
	m_pBtnNextPage->SetVisible(false);
	m_pBtnEndPage->SetVisible(false);
	//Hide all page button
	for (int i=1; i<=const_total_page_btn_count; i++)
	{
		char buf[100]={0};
		sprintf_s(buf, sizeof(buf), _T("btn_page_%d"), i);
		CControlUI* pControlPagebtn= (CControlUI*)static_cast<CControlUI*>(m_PaintManager->FindControl(buf));
		if (NULL != pControlPagebtn)
		{
			pControlPagebtn->SetVisible(false);
		}
	}

	//2:Get new project items;
	if (NULL != m_pProjProcessor)
	{
		if(m_pProjProcessor->GetProject(strStartDataTime, m_vecProjects,strSearch))
		{
			int nShowCount = 0;
			if (m_vecProjects.size()>const_max_items_in_page)
			{
				nShowCount = const_max_items_in_page;
			}
			else
			{
				nShowCount = m_vecProjects.size();
			}

			AddListItem(0, nShowCount);

			UpdateParas(m_vecProjects.size());

			UpdatePageButton();
		}
	}
}

//����������Ϣ��������Ӧ��ť����ʾ
void CProjectInfoPage::UpdateParas(int nCount)
{
	int nPageCount = nCount/const_max_items_in_page;
	int nLeftItemCount = nCount%const_max_items_in_page;
	if (nPageCount>0 )
	{
		if (nLeftItemCount>0)
		{
			nPageCount += 1;
		}

		//����Page��Ϣ
		m_nEndPageIndex = nPageCount;
		m_nCurrentPageIndex = 1;
	}
	else
	{
		m_nEndPageIndex = -1;
		m_nCurrentPageIndex = -1;
	}
}

//������ǰҳ
//���ҳ�泬��10ҳ����ʾ������Ϊ��m_nCurrentPageindex-5, m_nCurrentPageIndex+5
void CProjectInfoPage::UpdatePageButton()
{
	if (m_nEndPageIndex>0)
	{
		//1������ʾ��ť��
		int nButtonCount = 0;
		//1-10��
		if (m_nEndPageIndex<=const_total_page_btn_count)
		{
			nButtonCount = m_nEndPageIndex;
		}
		//>10��
		else
		{
			nButtonCount = const_total_page_btn_count;
		}
		
		//����ʾ4����ť��
		m_pBtnFirstpage->SetVisible(true);
		m_pBtnPrevPage->SetVisible(true);
		m_pBtnNextPage->SetVisible(true);
		m_pBtnEndPage->SetVisible(true);

		//��ʼ��1--10(���)��
		for (int i=1; i<=nButtonCount; i++)
		{
			char buf[100]={0};
			sprintf_s(buf, sizeof(buf), _T("btn_page_%d"), i);
			CControlUI* pControlPagebtn= (CControlUI*)static_cast<CControlUI*>(m_PaintManager->FindControl(buf));
			if (NULL != pControlPagebtn)
			{
				sprintf_s(buf, sizeof(buf), _T("%d"), i);
				pControlPagebtn->SetAttribute("textcolor", "#ff000000");
				pControlPagebtn->SetText(buf);
				pControlPagebtn->SetVisible(true);
			}
		}

		//2:���ݵ�ǰҳ��ֵ������Ƿ���Ҫ����ˢ�°�ť�ı���
		int nBeginIndex=0;
		int nEndindex=0;
		bool bNeedUpdate = false;
		//ֻ�ж���10ҳ�Ĳ��п���ˢ�°�ť
		if (m_nEndPageIndex > const_total_page_btn_count)
		{
			if (m_nCurrentPageIndex-5<=0)
			{
				nBeginIndex = 1;
				nEndindex   = const_total_page_btn_count;
			}
			else if (m_nCurrentPageIndex+5>=m_nEndPageIndex)
			{
				nBeginIndex = m_nEndPageIndex-9;
				nEndindex   = m_nEndPageIndex;
				//bNeedUpdate = true;
			}
			else
			{
				nBeginIndex = m_nCurrentPageIndex-4;
				nEndindex = m_nCurrentPageIndex+5;
				//bNeedUpdate = true;
			}

			//if (bNeedUpdate)
			{
				//���°�ť��ֵ
				for (int i=1; i<=const_total_page_btn_count; i++)
				{
					char buf[100]={0};
					sprintf_s(buf, sizeof(buf), _T("btn_page_%d"), i);
					CControlUI* pControlPagebtn= (CControlUI*)static_cast<CControlUI*>(m_PaintManager->FindControl(buf));
					if (NULL != pControlPagebtn)
					{
						memset(buf, 100, 0);
						sprintf_s(buf, sizeof(buf), _T("%d"), nBeginIndex++);
						pControlPagebtn->SetText(buf);
					}
				}
			}
		}

		//3:���ٸ�����ǰҳ��ť��
		for (int i=1; i<=nButtonCount; i++)
		{
			char buf[100]={0};
			sprintf_s(buf, sizeof(buf), _T("btn_page_%d"), i);
			CControlUI* pControlPagebtn= (CControlUI*)static_cast<CControlUI*>(m_PaintManager->FindControl(buf));
			if (NULL != pControlPagebtn)
			{
				memset(buf, 100, 0);
				sprintf_s(buf, sizeof(buf), _T("%d"), m_nCurrentPageIndex);

				tstring strBtn = pControlPagebtn->GetText();
				if (_tcsicmp(strBtn.c_str(), buf) == 0)
				{
					pControlPagebtn->SetAttribute("textcolor", "#ff21a7d4");
					pControlPagebtn->SetText(buf);
					break;
				}
			}
		}

		
		//ˢ��buttonλ��
		if (NULL != m_pBtnPrevPage)
		{
			//��һҳ
			SIZE szPos = m_pBtnPrevPage->GetFixedXY();
			int nBtnWidth = m_pBtnPrevPage->GetFixedWidth();
			szPos.cx = szPos .cx + nBtnWidth + nButtonCount*const_nButtonOffset + const_button_gap;
			m_pBtnNextPage->SetFixedXY(szPos);

			//βҳ
			szPos.cx += nBtnWidth+5;
			m_pBtnEndPage->SetFixedXY(szPos);
		}

	}	
	//û��ҳ��������ʾ�ĸ���ť
	else
	{
		if (NULL != m_pBtnPrevPage)
		{
			m_pBtnFirstpage->SetVisible(false);
			m_pBtnPrevPage->SetVisible(false);
			m_pBtnNextPage->SetVisible(false);
			m_pBtnEndPage->SetVisible(false);
			//��һҳ
// 			SIZE szPos = m_pBtnPrevPage->GetFixedXY();
// 			int nBtnWidth = m_pBtnPrevPage->GetFixedWidth();
// 			szPos.cx = szPos.cx + nBtnWidth + const_button_gap;
// 			m_pBtnNextPage->SetFixedXY(szPos);
// 
// 			//βҳ
// 			szPos.cx += nBtnWidth+5;
// 			m_pBtnEndPage->SetFixedXY(szPos);
		}

	}
}

void CProjectInfoPage::GotoPage(int nPageIndex)
{
	if (nPageIndex>0 && nPageIndex<= m_nEndPageIndex)
	{
		//����յ�ǰ��list
		RemoveAllListItems();

		int nStartIndexInCache = (nPageIndex-1)*const_max_items_in_page;
		int nShowCount = const_max_items_in_page;
		if (nPageIndex == m_nEndPageIndex)//��������һҳ:��ȥǰn-1ҳ����ʣ�µĵģ�1-8��
		{
			nShowCount = m_vecProjects.size() - (nPageIndex-1)*const_max_items_in_page;
		}

		AddListItem(nStartIndexInCache, nShowCount);

		m_nCurrentPageIndex = nPageIndex;

		//����ǰҳ��������Ϊ����
		UpdatePageButton();
	}
}

void CProjectInfoPage::RemoveAllListItems()
{
	int nCount = m_pListProjs->GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CListContainerElementUI* pListItem = static_cast<CListContainerElementUI*>(m_pListProjs->GetItemAt(i));
			if (NULL != pListItem)
			{
				sProjectItem* pProjData = (sProjectItem*)pListItem->GetTag();
				if (NULL != pProjData)
				{
					delete pProjData;
					pProjData = NULL;
				}
			}
		}

		m_pListProjs->RemoveAll();
	}

	//��ԭIndex
	m_nCurrentPageIndex = -1;
}
void CProjectInfoPage::AddListItem(int nFromIndex, int nCount)
{
	for (int i=nFromIndex; i<nCount+nFromIndex; i++)
	{
		CDialogBuilder DlgBuilder;
		CListContainerElementUI *plstItem = NULL;
		if (!DlgBuilder.GetMarkup()->IsValid())
		{
			plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create(_T("project_list_item.xml"),
				(UINT)0, NULL, m_PaintManager));
		}
		else
		{
			plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create((UINT)0, m_PaintManager));
		}
		if (plstItem == NULL)
			return ;
		else
		{
			sProjectItem& oneProj = m_vecProjects[i];
			sProjectItem* pNewData = new sProjectItem;
			pNewData->strCreateTime = oneProj.strCreateTime;
			pNewData->strDestOrgID = oneProj.strDestOrgID;
			pNewData->strDestOrgName = oneProj.strDestOrgName;
			pNewData->strProjID = oneProj.strProjID;
			pNewData->strProjName = oneProj.strProjName;
			pNewData->strRoomID = oneProj.strRoomID;
			pNewData->vecDestParticipants = oneProj.vecDestParticipants;

			CLabelUI *pLabelProjName = static_cast<CLabelUI *>(m_PaintManager->FindSubControlByName(plstItem, _T("lable_project_name")));
			if (pLabelProjName != NULL)
			{
				pLabelProjName->SetText(oneProj.strProjName.c_str());
			}

			tstring strShowDest;
			//�ж��Ǹ��˻��ǻ�����
			if (oneProj.strDestOrgID.empty() 
				&& oneProj.strDestOrgName.empty()
				&& oneProj.vecDestParticipants.size()>0)
			{
				tstring strAccount = oneProj.vecDestParticipants[0];
				strAccount += MMPC_ACCOUNT;
			

				//�����޸ģ�ȡ�����û��ı�ע��
				sUserVCard oneUser;
				if (GetDBTaskModule()->LoadUserVCard(strAccount,  oneUser))
				{
					strShowDest = oneUser.strUserNickname.c_str();
				}
				sUserVCard oneNewVCard;
				if(CController::Instance()->GetUserInfoMgr()->GetUserVCard(strAccount, oneNewVCard))
				{
					strShowDest = oneNewVCard.strUserNickname;
				}

				if (strShowDest.empty())
				{
					strShowDest = oneProj.vecDestParticipants[0];
				}
				
			}
			else
			{
				strShowDest = oneProj.strDestOrgName;
			}

			CLabelUI *pLabelOrgName = static_cast<CLabelUI *>(m_PaintManager->FindSubControlByName(plstItem, _T("label_orgnization_name")));
			if (pLabelOrgName != NULL)
			{
				pLabelOrgName->SetText(strShowDest.c_str());
			}

			CLabelUI *pLabelTime = static_cast<CLabelUI *>(m_PaintManager->FindSubControlByName(plstItem, _T("label_project_time")));
			if (pLabelTime != NULL)
			{
				pLabelTime->SetText(oneProj.strCreateTime.c_str());
			}

			//��roomID����ʶΨһ��Item
			plstItem->SetTag((UINT_PTR)pNewData);

			plstItem->SetFixedHeight(50);
			if (!m_pListProjs->Add(plstItem))
			{
				delete plstItem;
			}
		}
	}
}

bool CProjectInfoPage::IsCacheEmpty()
{
	if (m_vecProjects.size() == 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("ʧ��","û�п�����ʾ����Ŀ��", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();

		return true;
	}

	return false;
}

bool CProjectInfoPage::IsFirstPage()
{
	if (m_nCurrentPageIndex == 1)
	{
		CMsgBox* pMsgBox = new CMsgBox("����","��ǰ�Ѿ��ǵ�һҳ�ˣ�", MSGBOX_ICON_WARNING, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return true;
	}

	return false;
}

bool CProjectInfoPage::IsEndPage()
{
	if (m_nCurrentPageIndex == m_nEndPageIndex)
	{
		CMsgBox* pMsgBox = new CMsgBox("����","��ǰҳ�Ѿ������һҳ�ˣ�", MSGBOX_ICON_WARNING, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return true;
	}

	return false;
}