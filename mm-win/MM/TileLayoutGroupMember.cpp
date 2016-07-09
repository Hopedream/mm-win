#include "TileLayoutGroupMember.h"
#include "CDialogBuilderCallbackEx.h"
#include "Utility.h"
#include <sstream>
#include "mainDlg.h"

CTileLayoutGroupMember::CTileLayoutGroupMember(CPaintManagerUI* ppm)
{
	m_pPaintManager = ppm;


}

CTileLayoutGroupMember::~CTileLayoutGroupMember()
{

}

void CTileLayoutGroupMember::DoEvent(TEventUI& event)
{
	CTileLayoutUI::DoEvent(event);
}

//注意:由于该类的基类是TileLayout，所以create_project_opposite_employee.xml的父节点不可以是ListContainerElement，而应该是Container节点。否则会出现刷新List失败的问题。
void CTileLayoutGroupMember::AddOneItem(sFriendListItem& oneFriend,
	vector<tstring>&vecProperty, bool bDefault/*=false*/)
{
	SetItemSize(CDuiSize(290, 50));

	CDialogBuilder builder;
	CContainerUI* pOneEmployee = static_cast<CContainerUI*>(builder.Create(_T("groupmember_listitem.xml"), (UINT)0));
	if( pOneEmployee != NULL ) 
	{	
		if( pOneEmployee == NULL ) 
			pOneEmployee = static_cast<CContainerUI*>(builder.Create());

		if( pOneEmployee != NULL ) 
		{
			sFriendListItem* pFriendData = new sFriendListItem;
			pFriendData->eState = oneFriend.eState;
			pFriendData->nItemID = oneFriend.nItemID;
			pFriendData->strAccount = oneFriend.strAccount;
			pFriendData->strAvatar = oneFriend.strAvatar;
			pFriendData->strOrgID = oneFriend.strOrgID;
			pFriendData->strOrgnization = oneFriend.strOrgnization;
			pFriendData->strShowName = oneFriend.strShowName;
			
			pOneEmployee->SetTag((UINT_PTR)pFriendData);
			this->Add(pOneEmployee);
		}
		else 
		{
			this->RemoveAll();
			return;
		}

		//1. avatar
		CControlUI* pAvatar = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("groupmember_avatar")));
		if (NULL != pAvatar)
		{
			pAvatar->SetBkImage(oneFriend.strAvatar.c_str());

			
		}

		//2. 设置名字
		CLabelUI *pLabelShowName = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("groupmember_nickname")));
		if (NULL != pLabelShowName)
		{
			pLabelShowName->SetText(oneFriend.strShowName.c_str());

		}
		
		//3.设置属性
		vector<CLabelUI*> vect_label;
		for (int i=1;i<=4;i++)
		{
			std::ostringstream ostr;
			ostr << "groupmem_destduty_" << i;
			CLabelUI *pLabelProperty = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, ostr.str().c_str()));			
			if (NULL != pLabelProperty)
			{
				if (i>vecProperty.size())
				{
					pLabelProperty->SetVisible(false);
				}
				else
				{
					pLabelProperty->SetText(vecProperty[i-1].c_str());
					pLabelProperty->SetVisible(true);
					vect_label.push_back(pLabelProperty);
				}
			}
		}

		//位置移动。。。。。。。。。。。。by zyy
		//duilib无法获取控件位置，可以设置控件位置，因此，这里的位置是从xml里面得到
		//得到名字占用的位置
		RECT rect_origin, rect_total;
		rect_origin.left = 44;
		rect_origin.right = 151;
		rect_origin.top = 7;
		rect_origin.bottom = 22;

		rect_total.left = 44;
		rect_total.right = 280;
		rect_total.top = 7;
		rect_total.bottom = 22;

		//得到名字字符串的长度
		HDC hdc = m_pPaintManager->GetPaintDC();
		SIZE sz = CRenderEngine::GetTextSize(hdc, m_pPaintManager, pLabelShowName->GetText(), -1,0);
		sz.cx += 12;

		MoveControllers(pLabelShowName, rect_origin, sz, vect_label, rect_total);
		//////////////////////////////////////////////////////////////////////////
		

		////4.设置Duty
		//CLabelUI *pLableXuQiu = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("groupmem_destduty_xuqiu")));
		//if (vecProperty.size()>0)
		//{
		//	pLableXuQiu->SetText(vecProperty[0].c_str());
		//	pLableXuQiu->SetVisible(true);
		//}
		//else
		//{
		//	pLableXuQiu->SetVisible(false);
		//}

		//CLabelUI *pLableDingyue= static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("groupmem_destduty_dingyue")));
		//if (vecProperty.size()>=2)
		//{
		//	pLableDingyue->SetText(vecProperty[1].c_str());
		//	pLableDingyue->SetVisible(true);
		//}
		//else
		//{
		//	pLableDingyue->SetVisible(false);
		//}

		//CLabelUI *pLableZhixing = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("groupmem_destduty_zhixing")));
		//if (vecProperty.size()>=3)
		//{
		//	pLableZhixing->SetText(vecProperty[2].c_str());
		//	pLableZhixing->SetVisible(true);
		//}
		//else
		//{
		//	pLableZhixing->SetVisible(false);
		//}

		//CLabelUI *pLableShouhou = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("groupmem_destduty_shouhou")));
		//if (vecProperty.size()>=4)
		//{
		//	pLableShouhou->SetText(vecProperty[3].c_str());
		//	pLableShouhou->SetVisible(true);
		//}
		//else
		//{
		//	pLableShouhou->SetVisible(false);
		//}

		CLabelUI *pLabelOrgName = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("groupmem_orgname")));
		if (NULL != pLabelOrgName)
		{
			pLabelOrgName->SetText(oneFriend.strOrgnization.c_str());
		}
		
	}
}

void CTileLayoutGroupMember::DeSelectItem(tstring& strAccount)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CContainerUI* pListItem = static_cast<CContainerUI*>(GetItemAt(i));
			if (NULL != pListItem)
			{
				sFriendListItem* pFriend = (sFriendListItem*)pListItem->GetTag();
				if(_tcsicmp(pFriend->strAccount.c_str(), strAccount.c_str()) == 0)
				{
					COptionUI* pOption = static_cast<COptionUI*>(m_pPaintManager->FindSubControlByName(pListItem, _T("option_oppo_employe_selected")));
					if (NULL != pOption)
					{
						pOption->Selected(false);
						break;
					}
				}
			}
		}
	}
}

void CTileLayoutGroupMember::SetItemSeleted(tstring& strAccount)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CContainerUI* pListItem = static_cast<CContainerUI*>(GetItemAt(i));
			if (NULL != pListItem)
			{
				sFriendListItem* pFriend = (sFriendListItem*)pListItem->GetTag();
				if(_tcsicmp(pFriend->strAccount.c_str(), strAccount.c_str()) == 0)
				{
					COptionUI* pOption = static_cast<COptionUI*>(m_pPaintManager->FindSubControlByName(pListItem, _T("option_oppo_employe_selected")));
					if (NULL != pOption)
					{
						pOption->Selected(true);
						break;
					}
				}
			}
		}
	}
}