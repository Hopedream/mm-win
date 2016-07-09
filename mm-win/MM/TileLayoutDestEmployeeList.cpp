#include "TileLayoutDestEmployeeList.h"
#include "CDialogBuilderCallbackEx.h"
#include "Utility.h"
#include <sstream>
#include "mainDlg.h"

CTileLayoutDestEmployeeList::CTileLayoutDestEmployeeList(CPaintManagerUI* ppm)
{
	m_pPaintManager = ppm;
}

CTileLayoutDestEmployeeList::~CTileLayoutDestEmployeeList()
{

}

void CTileLayoutDestEmployeeList::DoEvent(TEventUI& event)
{
	CTileLayoutUI::DoEvent(event);
}

//注意:由于该类的基类是TileLayout，所以create_project_opposite_employee.xml的父节点不可以是ListContainerElement，而应该是Container节点。否则会出现刷新List失败的问题。
//void CTileLayoutDestEmployeeList::AddOneItem(tstring& strAccount, tstring& strAvatar, tstring& strShowName, bool bDefault/*=false*/)
void CTileLayoutDestEmployeeList::AddOneItem(sFriendListItem& oneFriend,vector<tstring>&vecProperty, bool bDefault/*=false*/)
{
	SetItemSize(CDuiSize(185, 50));

	CDialogBuilder builder;
	CContainerUI* pOneEmployee = static_cast<CContainerUI*>(builder.Create(_T("create_project_opposite_employee.xml"), (UINT)0));
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
			//pOneEmployee->SetTag(GetNextItemID());
			pOneEmployee->SetTag((UINT_PTR)pFriendData);
			this->Add(pOneEmployee);
		}
		else 
		{
			this->RemoveAll();
			return;
		}

		//1. avatar
		CControlUI* pAvatar = static_cast<CControlUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("control_oppo_unselect_avatar")));
		if (NULL != pAvatar)
		{
			pAvatar->SetBkImage(oneFriend.strAvatar.c_str());
		}

		//2. 设置名字
		CLabelUI *pLabelShowName = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("label_oppo_unselect_name")));
		if (NULL != pLabelShowName)
		{
			pLabelShowName->SetText(oneFriend.strShowName.c_str());
		}

		//3.设置属性
		for (int i=1;i<=4;i++)
		{
			std::ostringstream ostr;
			ostr << "destduty_" << i;
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

		//设置默认接收人
		tstring recipient_id;
		CLabelUI* pLabelDefault = NULL;
		if (GetRecipient(oneFriend.strOrgID, recipient_id))
		{
			if (!recipient_id.empty())
			{
				int find_num = oneFriend.strAccount.find(recipient_id);
				if (find_num >= 0)//当前就是默认接收人
				{
					pLabelDefault = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("label_oppo_unselect_isDefault")));
					pLabelDefault->SetVisible(true);
				}
			}
		}

		//只有在出现默认接收人的时候才需要考虑位置移动
		//位置移动。。。。。。。。。。。。by zyy
		//duilib无法获取控件位置，可以设置控件位置，因此，这里的位置是从xml里面得到
		//得到名字占用的位置
		if (pLabelDefault && m_pPaintManager)
		{
			RECT rect_origin, rect_default, rect_total;
			rect_origin.left = 47;
			rect_origin.right = 167;
			rect_origin.top = 8;
			rect_origin.bottom = 23;

			rect_default.left = 102;
			rect_default.right = 172;
			rect_default.top = 8;
			rect_default.bottom = 23;

			rect_total.left = 47;
			rect_total.right = 180;
			rect_total.top = 8;
			rect_total.bottom = 23;

			//得到名字字符串的长度
			HDC hdc;
			hdc = m_pPaintManager->GetPaintDC();
			
			SIZE sz_name = CRenderEngine::GetTextSize(hdc, m_pPaintManager, pLabelShowName->GetText(), -1,0);
			sz_name.cx += 12;

			SIZE sz_default = CRenderEngine::GetTextSize(hdc, m_pPaintManager, pLabelDefault->GetText(), -1,0);
			sz_default.cx += 12;

			MoveControllers(pLabelShowName, rect_origin, sz_name, 
				pLabelDefault, rect_default, sz_default,
				rect_total);
		}
		//////////////////////////////////////////////////////////////////////////


		//3. 
		/*CLabelUI* pLabelDefault = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("label_oppo_unselect_isDefault")));
		if (NULL != pLabelDefault)
		{
			pLabelDefault->SetVisible(bDefault);
		}*/

		////4.设置Duty
		//CLabelUI *pLableXuQiu = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("destduty_xuqiu")));
		//if (vecProperty.size()>0)
		//{
		//	pLableXuQiu->SetText(vecProperty[0].c_str());
		//	pLableXuQiu->SetVisible(true);
		//}
		//else
		//{
		//	pLableXuQiu->SetVisible(false);
		//}

		//CLabelUI *pLableDingyue= static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("destduty_dingyue")));
		//if (vecProperty.size()>=2)
		//{
		//	pLableDingyue->SetText(vecProperty[1].c_str());
		//	pLableDingyue->SetVisible(true);
		//}
		//else
		//{
		//	pLableDingyue->SetVisible(false);
		//}

		//CLabelUI *pLableZhixing = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("destduty_zhixing")));
		//if (vecProperty.size()>=3)
		//{
		//	pLableZhixing->SetText(vecProperty[2].c_str());
		//	pLableZhixing->SetVisible(true);
		//}
		//else
		//{
		//	pLableZhixing->SetVisible(false);
		//}

		//CLabelUI *pLableShouhou = static_cast<CLabelUI*>(m_pPaintManager->FindSubControlByName(pOneEmployee, _T("destduty_shouhou")));
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

void CTileLayoutDestEmployeeList::DeSelectItem(tstring& strAccount)
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

void CTileLayoutDestEmployeeList::SetItemSeleted(tstring& strAccount)
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

// 未选择对方参与者时,选择参与者,(无 -> 任一离线人员 -> 默认接收方(离线) -> 任一在线人员 -> 默认接收方(在线))
void CTileLayoutDestEmployeeList::GetDestParticipant(vector<tstring>& vecParticipant)
{
	bool isFirst = true;
	tstring strRecipient;
	vector<tstring> vecTemp,vecTempOffline;
	int nDestCount = GetCount();
	if(nDestCount > 0)
	{
		for (int i=0; i<nDestCount; i++)
		{
			CContainerUI* pListItem = static_cast<CContainerUI*>(GetItemAt(i));
			sFriendListItem* pOneFriend = (sFriendListItem* )pListItem->GetTag();
			if (NULL != pOneFriend)
			{
				if(isFirst)
				{
					isFirst = false;
					// 获取机构中的默认接受者:如果是个人业务,必须选择指定的好友,否则直接返回
					if(_tcsicmp(pOneFriend->strOrgID.c_str(),"-1") == 0 && _tcsicmp(pOneFriend->strOrgnization.c_str(),"我的好友") == 0)
						return;
					else
						GetRecipient(pOneFriend->strOrgID,strRecipient);
				}

				if(strRecipient.length() > 0)
				{
					if(pOneFriend->eState != enmState_Offline && _tcsicmp(strRecipient.c_str(), pOneFriend->strAccount.c_str()) == 0)
					{
						// 如果存在默认接受者且在线,将参与方设置为默认接受者,此优先级最高
						vecParticipant.push_back(strRecipient);
						return;
					}
				}
				if(pOneFriend->eState != enmState_Offline)
					vecTemp.push_back(pOneFriend->strAccount);
				else
					vecTempOffline.push_back(pOneFriend->strAccount);
			}
		}

		if(vecTemp.size() > 0)
		{
			// 如果默认接受者不在线,则将当前在线用户之一设置为参与方
			vecParticipant.push_back(vecTemp[0]);
		}
		else if(strRecipient.length() > 0)
		{
			// 如果无在线人员,则将默认接受人设置为参与方
			vecParticipant.push_back(strRecipient);
		}
		else if(vecTempOffline.size() > 0)
		{
			// 如果不存在默认接受者,则将任一成员设置成参与方
			vecParticipant.push_back(vecTempOffline[0]);
		}
		// 如果该机构无人,则不设置参与方
	}
}