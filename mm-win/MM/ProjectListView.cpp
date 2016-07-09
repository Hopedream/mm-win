#include "ProjectListView.h"
#include "Controller.h"
#include "mainDlg.h"
#include "MUCRoomMgr.h"
#include "Utility.h"
#include <vector>
#include "HttpClient.h"
#include "CDialogBuilderCallbackEx.h"
#include "AddNewGroupMember.h"
#include "MsgBox.h"
#include "..\include\DBTaskModule.h"
#include "Controller.h"
#include "UserManager.h"
#include "MUCRoomMgr.h"

//#include "ProjectsListPageEx.h"

const int const_total_page_btn_count = 10;
const int const_max_items_in_page = 8;
const int const_one_week_seconds  = 7*24*3600;
const int const_one_month_seconds = 30*24*3600;//暂时就取一个月平均值30天计算
const int const_nButtonOffset	  = 33;
const int const_button_gap		  = 20;


CProjectListView::CProjectListView(CPaintManagerUI* ppm/* = NULL*/)
	:m_pListProjs(NULL)
	,m_pProjProcessor(NULL)
	//,m_pAddGroupMemWnd(NULL)
	,m_pBtnPrevPage(NULL)
	,m_pBtnNextPage(NULL)
	,m_pBtnEndPage(NULL)
	/*,m_pProjPage(NULL)*/
	,m_nCurrentPageIndex(-1)
	,m_nEndPageIndex(-1)
	,m_bOrgId(false)
{

}

CProjectListView::~CProjectListView()
{

}

void CProjectListView::Notify(TNotifyUI& msg)
{
// 	if (0 == _tcsicmp(DUI_MSGTYPE_WINDOWINIT, msg.sType))
// 	{
// 		HandleWindowInitEvent(msg);
// 	}

	if (msg.sType != DUI_MSGTYPE_CLICK )
	{
		return;
	}

	if (_tcsicmp(msg.sType, _T("click")) == 0)
	{
		DuiLib::CDuiString strName = msg.pSender->GetName();

		//关闭界面
		if (_tcsicmp(msg.pSender->GetName(), "btn_bnlist_close") == 0)
		{
			ShowWindow(false);
			//Close();
		}
		else if (_tcsicmp(msg.pSender->GetName(), "btn_bnlist_mini") == 0)
		{
			::ShowWindow(*this, SW_MAXIMIZE);
		}
		//"第一页"
		else if (_tcsicmp(strName.GetData(), "btn_page_first") == 0)
		{
			if (!IsCacheEmpty()&& !IsFirstPage())
			{
				GotoPage(1);
			}	
		}
		//"上一页"
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
		//"下一页"
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
		//"最后一页"
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
		//1--10页按钮
		else if (strName.Find("btn_page_")>=0)
		{
			int nIndex = atoi(msg.pSender->GetText());
			GotoPage(nIndex);
			return;
		}
		else if (msg.pSender->GetName() == "btn_get_project_info")
		{
			sProjectItem* pData = (sProjectItem*)msg.pSender->GetParent()->GetParent()->GetParent()->GetTag();
			if (NULL != pData)
			{

			}
		}
		else if (msg.pSender->GetName() == "btn_bussiness_chat")
		{
			sProjectItem* pData = (sProjectItem*)msg.pSender->GetParent()->GetParent()->GetParent()->GetTag();
			if (NULL != pData)
			{
				//没有创建房间，就需要重新选人：既然能看到这个项目，说明这个项目就是我创建的，所以是可以选人。
				if (pData->strRoomID.empty())
				{
					
					CAddNewGroupMember* pAddGroupMemWnd = new CAddNewGroupMember;
					if (NULL != pAddGroupMemWnd)
					{
						pAddGroupMemWnd->CreateWnd();
						pAddGroupMemWnd->Initialize();//初始化一次。
					}

					pAddGroupMemWnd->SetResProperty(false, pData->strProjID, pData->strProjName);
					pAddGroupMemWnd->ShowWindow(true);
				}
				else
				{
					//我有可能不是这个群里面的成员，就需要判断下。如果我不是这个群里面的人，就不能进去。
					tstring strMyPhone = CController::Instance()->GetXmppClient()->jid().username();
					tstring strMyAccount = strMyPhone + MMPC_ACCOUNT;
					if(CController::Instance()->GetRoomManager()->IsInThisRoom(pData->strRoomID, strMyAccount))
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
						CMsgBox* pMsgBox = new CMsgBox("错误","您不是该群组成员！");
						pMsgBox->ShowModal();
					}
				}
			}		
		}
	}
}
//初始化窗口
void CProjectListView::InitWindow()
{
	
	m_pListProjs = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listProjs_ex")));
	m_pBtnNextPage = static_cast<CButtonUI*>(m_PaintManager.FindControl("btn_page_next"));
	m_pBtnEndPage  = static_cast<CButtonUI*>(m_PaintManager.FindControl("btn_page_end"));
	m_pBtnPrevPage = static_cast<CButtonUI*>(m_PaintManager.FindControl("btn_page_prev"));
	m_pListProjs = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listProjs_ex")));

	//创建项目逻辑模块
	if (NULL == m_pProjProcessor)
	{
		m_pProjProcessor = new CProjectProcessor;
	}
}

LRESULT CProjectListView::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	bHandled = FALSE;
	return 0;
}

CControlUI* CProjectListView::CreateControl(LPCTSTR pstrClass)
{
// 	if (_tcsicmp(pstrClass, "ListProjs") == 0)
// 	{
// 		
// 	}
	return NULL;
}
bool CProjectListView::Initialize(tstring& strOrgIdOrAcount, bool bOrgID)
{
	m_strCurrAccount = strOrgIdOrAcount;
	m_bOrgId = bOrgID;

	//获取项目列表。
	FetchProjects();

	return true;
}

void CProjectListView::CreateWnd()
{
	Create(NULL, _T("MM1.0"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 600, 800);		
	CenterWindow();	
}

bool CProjectListView::FetchProjects()
{
	//1:Remove all first.
	RemoveAllListItems();

	//Hide all page button
	for (int i=1; i<=const_total_page_btn_count; i++)
	{
		char buf[100]={0};
		sprintf_s(buf, sizeof(buf), _T("btn_page_%d"), i);
		CControlUI* pControlPagebtn= (CControlUI*)static_cast<CControlUI*>(m_PaintManager.FindControl(buf));
		if (NULL != pControlPagebtn)
		{
			pControlPagebtn->SetVisible(false);
		}
	}

	//2:Get new project items;
	bool bFetchSucc = false;
	if (NULL != m_pProjProcessor && !m_strCurrAccount.empty())
	{
		if (m_bOrgId)
		{
			bFetchSucc = m_pProjProcessor->GetProjectsByOrgId(m_strCurrAccount, m_vecProjects);
		}

		else 
		{
			bFetchSucc = m_pProjProcessor->GetProjectsByCellPhone(m_strCurrAccount, m_vecProjects);
		}

		if (bFetchSucc)
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

	return bFetchSucc;
}

//更新索引信息，控制相应按钮的显示
void CProjectListView::UpdateParas(int nCount)
{
	int nPageCount = nCount/const_max_items_in_page;
	int nLeftItemCount = nCount%const_max_items_in_page;
	if (nPageCount>0 )
	{
		if (nLeftItemCount>0)
		{
			nPageCount += 1;
		}

		//更新Page信息
		m_nEndPageIndex = nPageCount;
		m_nCurrentPageIndex = 1;
	}
	else
	{
		m_nEndPageIndex = -1;
		m_nCurrentPageIndex = -1;
	}
}

//高亮当前页
//如果页面超过10页。显示的区间为：m_nCurrentPageindex-5, m_nCurrentPageIndex+5
void CProjectListView::UpdatePageButton()
{
	if (m_nEndPageIndex>0)
	{
		//1：先显示按钮：
		int nButtonCount = 0;
		//1-10个
		if (m_nEndPageIndex<=const_total_page_btn_count)
		{
			nButtonCount = m_nEndPageIndex;
		}
		//>10个
		else
		{
			nButtonCount = const_total_page_btn_count;
		}
		//初始化1--10(最多)个
		for (int i=1; i<=nButtonCount; i++)
		{
			char buf[100]={0};
			sprintf_s(buf, sizeof(buf), _T("btn_page_%d"), i);
			CControlUI* pControlPagebtn= (CControlUI*)static_cast<CControlUI*>(m_PaintManager.FindControl(buf));
			if (NULL != pControlPagebtn)
			{
				sprintf_s(buf, sizeof(buf), _T("%d"), i);
				pControlPagebtn->SetAttribute("textcolor", "#ff000000");
				pControlPagebtn->SetText(buf);
				pControlPagebtn->SetVisible(true);
			}
		}

		//2:根据当前页数值，检查是否需要重新刷新按钮文本，
		int nBeginIndex=0;
		int nEndindex=0;
		bool bNeedUpdate = false;
		//只有多余10页的才有可能刷新按钮
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
				//更新按钮数值
				for (int i=1; i<=const_total_page_btn_count; i++)
				{
					char buf[100]={0};
					sprintf_s(buf, sizeof(buf), _T("btn_page_%d"), i);
					CControlUI* pControlPagebtn= (CControlUI*)static_cast<CControlUI*>(m_PaintManager.FindControl(buf));
					if (NULL != pControlPagebtn)
					{
						memset(buf, 100, 0);
						sprintf_s(buf, sizeof(buf), _T("%d"), nBeginIndex++);
						pControlPagebtn->SetText(buf);
					}
				}
			}
		}

		//3:并再高亮当前页按钮！
		for (int i=1; i<=nButtonCount; i++)
		{
			char buf[100]={0};
			sprintf_s(buf, sizeof(buf), _T("btn_page_%d"), i);
			CControlUI* pControlPagebtn= (CControlUI*)static_cast<CControlUI*>(m_PaintManager.FindControl(buf));
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


		//刷新button位置
		if (NULL != m_pBtnPrevPage)
		{
			//下一页
			SIZE szPos = m_pBtnPrevPage->GetFixedXY();
			int nBtnWidth = m_pBtnPrevPage->GetFixedWidth();
			szPos.cx = szPos .cx + nBtnWidth + nButtonCount*const_nButtonOffset + const_button_gap;
			m_pBtnNextPage->SetFixedXY(szPos);

			//尾页
			szPos.cx += nBtnWidth+5;
			m_pBtnEndPage->SetFixedXY(szPos);
		}

	}	
	//没有页数，就显示四个按钮
	else
	{
		if (NULL != m_pBtnPrevPage)
		{
			//下一页
			SIZE szPos = m_pBtnPrevPage->GetFixedXY();
			int nBtnWidth = m_pBtnPrevPage->GetFixedWidth();
			szPos.cx = szPos.cx + nBtnWidth + const_button_gap;
			m_pBtnNextPage->SetFixedXY(szPos);

			//尾页
			szPos.cx += nBtnWidth+5;
			m_pBtnEndPage->SetFixedXY(szPos);
		}

	}
}

void CProjectListView::GotoPage(int nPageIndex)
{
	if (nPageIndex>0 && nPageIndex<= m_nEndPageIndex)
	{
		//先清空当前的list
		RemoveAllListItems();

		int nStartIndexInCache = (nPageIndex-1)*const_max_items_in_page;
		int nShowCount = const_max_items_in_page;
		if (nPageIndex == m_nEndPageIndex)//如果是最后一页:减去前n-1页就是剩下的的：1-8个
		{
			nShowCount = m_vecProjects.size() - (nPageIndex-1)*const_max_items_in_page;
		}

		AddListItem(nStartIndexInCache, nShowCount);

		m_nCurrentPageIndex = nPageIndex;

		//将当前页的字体设为高亮
		UpdatePageButton();
	}
}

void CProjectListView::RemoveAllListItems()
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

	//还原Index
	m_nCurrentPageIndex = -1;
}
void CProjectListView::AddListItem(int nFromIndex, int nCount)
{
	for (int i=nFromIndex; i<nCount+nFromIndex; i++)
	{
		CDialogBuilder DlgBuilder;
		CListContainerElementUI *plstItem = NULL;
		if (!DlgBuilder.GetMarkup()->IsValid())
		{
			plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create(_T("project_list_item.xml"),
				(UINT)0, NULL, &m_PaintManager));
		}
		else
		{
			plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create((UINT)0, &m_PaintManager));
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

			CLabelUI *pLabelProjName = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("lable_project_name")));
			if (pLabelProjName != NULL)
			{
				pLabelProjName->SetText(oneProj.strProjName.c_str());
			}

			tstring strShowDest;
			//判断是个人还是机构。
			if (oneProj.strDestOrgID.empty() 
				&& oneProj.strDestOrgName.empty()
				&& oneProj.vecDestParticipants.size()>0)
			{
				tstring strAccount = oneProj.vecDestParticipants[0];
				strAccount += MMPC_ACCOUNT;

				//策略修改：取消了用户的备注名
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


			CLabelUI *pLabelOrgName = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("label_orgnization_name")));
			if (pLabelOrgName != NULL)
			{
				pLabelOrgName->SetText(strShowDest.c_str());
			}

			CLabelUI *pLabelTime = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("label_project_time")));
			if (pLabelTime != NULL)
			{
				pLabelTime->SetText(oneProj.strCreateTime.c_str());
			}

			//用roomID来标识唯一的Item
			plstItem->SetTag((UINT_PTR)pNewData);

			plstItem->SetFixedHeight(50);
			if (!m_pListProjs->Add(plstItem))
			{
				delete plstItem;
			}
		}
	}
}

bool CProjectListView::IsCacheEmpty()
{
	if (m_vecProjects.size() == 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("失败","没有可以显示的项目！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();

		return true;
	}

	return false;
}

bool CProjectListView::IsFirstPage()
{
	if (m_nCurrentPageIndex == 1)
	{
		CMsgBox* pMsgBox = new CMsgBox("提醒","当前已经是第一页了！", MSGBOX_ICON_WARNING, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return true;
	}

	return false;
}

bool CProjectListView::IsEndPage()
{
	if (m_nCurrentPageIndex == m_nEndPageIndex)
	{
		CMsgBox* pMsgBox = new CMsgBox("提醒","当前页已经是最后一页了！", MSGBOX_ICON_WARNING, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return true;
	}

	return false;
}