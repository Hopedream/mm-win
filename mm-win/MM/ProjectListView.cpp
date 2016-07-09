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
const int const_one_month_seconds = 30*24*3600;//��ʱ��ȡһ����ƽ��ֵ30�����
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

		//�رս���
		if (_tcsicmp(msg.pSender->GetName(), "btn_bnlist_close") == 0)
		{
			ShowWindow(false);
			//Close();
		}
		else if (_tcsicmp(msg.pSender->GetName(), "btn_bnlist_mini") == 0)
		{
			::ShowWindow(*this, SW_MAXIMIZE);
		}
		//"��һҳ"
		else if (_tcsicmp(strName.GetData(), "btn_page_first") == 0)
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
				//û�д������䣬����Ҫ����ѡ�ˣ���Ȼ�ܿ��������Ŀ��˵�������Ŀ�����Ҵ����ģ������ǿ���ѡ�ˡ�
				if (pData->strRoomID.empty())
				{
					
					CAddNewGroupMember* pAddGroupMemWnd = new CAddNewGroupMember;
					if (NULL != pAddGroupMemWnd)
					{
						pAddGroupMemWnd->CreateWnd();
						pAddGroupMemWnd->Initialize();//��ʼ��һ�Ρ�
					}

					pAddGroupMemWnd->SetResProperty(false, pData->strProjID, pData->strProjName);
					pAddGroupMemWnd->ShowWindow(true);
				}
				else
				{
					//���п��ܲ������Ⱥ����ĳ�Ա������Ҫ�ж��¡�����Ҳ������Ⱥ������ˣ��Ͳ��ܽ�ȥ��
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
						CMsgBox* pMsgBox = new CMsgBox("����","�����Ǹ�Ⱥ���Ա��");
						pMsgBox->ShowModal();
					}
				}
			}		
		}
	}
}
//��ʼ������
void CProjectListView::InitWindow()
{
	
	m_pListProjs = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listProjs_ex")));
	m_pBtnNextPage = static_cast<CButtonUI*>(m_PaintManager.FindControl("btn_page_next"));
	m_pBtnEndPage  = static_cast<CButtonUI*>(m_PaintManager.FindControl("btn_page_end"));
	m_pBtnPrevPage = static_cast<CButtonUI*>(m_PaintManager.FindControl("btn_page_prev"));
	m_pListProjs = static_cast<CListUI*>(m_PaintManager.FindControl(_T("listProjs_ex")));

	//������Ŀ�߼�ģ��
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

	//��ȡ��Ŀ�б�
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

//����������Ϣ��������Ӧ��ť����ʾ
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
void CProjectListView::UpdatePageButton()
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
		//��ʼ��1--10(���)��
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

		//3:���ٸ�����ǰҳ��ť��
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
			//��һҳ
			SIZE szPos = m_pBtnPrevPage->GetFixedXY();
			int nBtnWidth = m_pBtnPrevPage->GetFixedWidth();
			szPos.cx = szPos.cx + nBtnWidth + const_button_gap;
			m_pBtnNextPage->SetFixedXY(szPos);

			//βҳ
			szPos.cx += nBtnWidth+5;
			m_pBtnEndPage->SetFixedXY(szPos);
		}

	}
}

void CProjectListView::GotoPage(int nPageIndex)
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

	//��ԭIndex
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

bool CProjectListView::IsCacheEmpty()
{
	if (m_vecProjects.size() == 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("ʧ��","û�п�����ʾ����Ŀ��", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();

		return true;
	}

	return false;
}

bool CProjectListView::IsFirstPage()
{
	if (m_nCurrentPageIndex == 1)
	{
		CMsgBox* pMsgBox = new CMsgBox("����","��ǰ�Ѿ��ǵ�һҳ�ˣ�", MSGBOX_ICON_WARNING, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return true;
	}

	return false;
}

bool CProjectListView::IsEndPage()
{
	if (m_nCurrentPageIndex == m_nEndPageIndex)
	{
		CMsgBox* pMsgBox = new CMsgBox("����","��ǰҳ�Ѿ������һҳ�ˣ�", MSGBOX_ICON_WARNING, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return true;
	}

	return false;
}