#include "TileLayoutDestEmployeeSelectedList.h"
#include "CDialogBuilderCallbackEx.h"
#include "Utility.h"
#include <sstream>

CTileLayoutDestEmployeeSelectedList::CTileLayoutDestEmployeeSelectedList(CPaintManagerUI* ppm)
{
	m_pPaintManager = ppm;
}

CTileLayoutDestEmployeeSelectedList::~CTileLayoutDestEmployeeSelectedList()
{

}

void CTileLayoutDestEmployeeSelectedList::DoEvent(TEventUI& event)
{
	CTileLayoutUI::DoEvent(event);
}

void CTileLayoutDestEmployeeSelectedList::RemoveOneItem(tstring& strAccount)
{
	int nCount = GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CContainerUI* pListItem = static_cast<CContainerUI*>(GetItemAt(i));
			sFriendListItem* pOneFriend = (sFriendListItem* )pListItem->GetTag();
			if (_tcsicmp(pOneFriend->strAccount.c_str(), strAccount.c_str()) == 0)
			{
				//remove from cache;
				for (map<tstring, sFriendListItem>::iterator it = m_mapAccount2Friend.begin();
					it != m_mapAccount2Friend.end(); ++it)
				{
					if (_tcsicmp(it->first.c_str(), pOneFriend->strAccount.c_str()) == 0)
					{
						m_mapAccount2Friend.erase(it);
						break;
					}
				}

				delete pOneFriend;
				pOneFriend = NULL;

				Remove(pListItem);
				break;
			}
		}
	}
}

void CTileLayoutDestEmployeeSelectedList::AddOneItem(sFriendListItem* pOneFriend, vector<tstring>& vecProperty)
{
	if (CheckIfExist(pOneFriend->strAccount))
	{
		return;
	}

	SetItemSize(CDuiSize(155, 50));

	CDialogBuilder builder;
	CContainerUI* pOneEmployee = static_cast<CContainerUI*>(builder.Create(_T("create_project_opposite_employee_selected.xml"), (UINT)0));
	if( pOneEmployee != NULL ) 
	{	
		if( pOneEmployee == NULL ) 
			pOneEmployee = static_cast<CContainerUI*>(builder.Create());

		if( pOneEmployee != NULL ) 
		{
 			sFriendListItem* pNewPartcipant = new sFriendListItem;
 			pNewPartcipant->eState = pOneFriend->eState;
			pNewPartcipant->nItemID = GetNextItemID();
 			pNewPartcipant->strAccount = pOneFriend->strAccount;
 			pNewPartcipant->strAvatar = pOneFriend->strAvatar;
 			pNewPartcipant->strOrgID = pOneFriend->strOrgID;
 			pNewPartcipant->strOrgnization = pOneFriend->strOrgnization;
 			pNewPartcipant->strShowName = pOneFriend->strShowName;
			
			pOneEmployee->SetTag((UINT_PTR)pNewPartcipant);

			this->Add(pOneEmployee);

			sFriendListItem oneFriend;
			oneFriend.eState = pNewPartcipant->eState;
			oneFriend.nItemID = pNewPartcipant->eState;
			oneFriend.strAccount = pNewPartcipant->strAccount;
			oneFriend.strAvatar = pNewPartcipant->strAvatar;
			oneFriend.strOrgID = pNewPartcipant->strOrgID;
			oneFriend.strOrgnization = pNewPartcipant->strOrgnization;
			oneFriend.strShowName = pNewPartcipant->strShowName;
			m_mapAccount2Friend.insert(make_pair(oneFriend.strAccount, oneFriend));
			
		}
		else 
		{
			this->RemoveAll();
			return;
		}

		//1. 设置avatar
		CControlUI* pAvatar = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("control_oppo_selected_employee_avatar")));
		if (NULL != pAvatar)
		{
			pAvatar->SetBkImage(pOneFriend->strAvatar.c_str());
		}

		//2. 设置名字
		CLabelUI *pLableFileName = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("label_oppo_selected_employee")));
		pLableFileName->SetText(pOneFriend->strShowName.c_str());


		//3.设置属性
		for (int i=1;i<=4;i++)
		{
			std::ostringstream ostr;
			ostr << "selectduty_" << i;
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
				}
			}
		}
		////设置Duty	
		//CLabelUI *pLableXuQiu = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("selectduty_xuqiu")));
		//if (vecProperty.size()>0)
		//{
		//	pLableXuQiu->SetText(vecProperty[0].c_str());
		//	pLableXuQiu->SetVisible(true);
		//}
		//else
		//{
		//	pLableXuQiu->SetVisible(false);
		//}

		//
		//CLabelUI *pLableDingyue= static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("selectduty_dingyue")));
		//if (vecProperty.size()>=2)
		//{
		//	pLableDingyue->SetText(vecProperty[1].c_str());
		//	pLableDingyue->SetVisible(true);
		//}
		//else
		//{
		//	pLableDingyue->SetVisible(false);
		//}
		//
		//CLabelUI *pLableZhixing = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("selectduty_zhixing")));
		//if (vecProperty.size()>=3)
		//{
		//	pLableZhixing->SetText(vecProperty[2].c_str());
		//	pLableZhixing->SetVisible(true);
		//}
		//else
		//{
		//	pLableZhixing->SetVisible(false);
		//}
		//

		//CLabelUI *pLableShouhou = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("selectduty_shouhou")));
		//if (vecProperty.size()>=4)
		//{
		//	pLableShouhou->SetText(vecProperty[3].c_str());
		//	pLableShouhou->SetVisible(true);
		//}
		//else
		//{
		//	pLableShouhou->SetVisible(false);
		//}
		
	}
}

bool CTileLayoutDestEmployeeSelectedList::CheckIfExist(tstring& strAccount)
{
	map<tstring, sFriendListItem>::iterator itFind = m_mapAccount2Friend.find(strAccount);
	if (itFind != m_mapAccount2Friend.end())
	{
		return true;
	}

	return false;
}

void CTileLayoutDestEmployeeSelectedList::GetSelectedOrgIDs(vector<tstring>& vecOrgIDs)
{
	for(map<tstring, sFriendListItem>::iterator it = m_mapAccount2Friend.begin();
		it != m_mapAccount2Friend.end(); ++it)
	{
		//if (vecOrgIDs.size()>0)
		//{
			bool bExist = false;
			for (vector<tstring>::iterator itChild = vecOrgIDs.begin(); 
				itChild != vecOrgIDs.end(); ++itChild)
			{
				if (_tcsicmp(itChild->c_str(), it->second.strOrgID.c_str()) == 0)
				{
					bExist = true;
					break;
				}
			}

			if (!bExist)
			{
				vecOrgIDs.push_back(it->second.strOrgID);
			}
		//}
	}
}

void CTileLayoutDestEmployeeSelectedList::GetDestParticipant(vector<tstring>& vecParticipant)
{
	int nDestCount = GetCount();
	if (nDestCount>0)
	{
		for (int i=0; i<nDestCount; i++)
		{
			CContainerUI* pListItem = static_cast<CContainerUI*>(GetItemAt(i));
			sFriendListItem* pOneFriend = (sFriendListItem* )pListItem->GetTag();
			if (NULL != pOneFriend)
			{
				vecParticipant.push_back(pOneFriend->strAccount);
			}
		}
	}
}

void CTileLayoutDestEmployeeSelectedList::GetOrgInfoByAccount(const tstring& strAccount, tstring&strOrgID, tstring& strOrgName)
{
	int nDestCount = GetCount();
	if (nDestCount>0)
	{
		for (int i=0; i<nDestCount; i++)
		{
			CContainerUI* pListItem = static_cast<CContainerUI*>(GetItemAt(i));
			sFriendListItem* pOneFriend = (sFriendListItem* )pListItem->GetTag();
			if ( NULL != pOneFriend && 
				_tcsicmp(strAccount.c_str(), pOneFriend->strAccount.c_str()) == 0)
			{
				strOrgID = pOneFriend->strOrgID;
				strOrgName = pOneFriend->strOrgnization;
			}
		}
	}
}