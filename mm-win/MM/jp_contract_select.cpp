#include "jp_contract_select.h"
#include "CDialogBuilderCallbackEx.h"
#include "MsgBox.h"
#include "webClient.h"
#include "..\json\json.h"
#include "..\include\Global_Def.h"
#include "..\Common\Logger.h"
#include "TileLayoutResPacket.h"

#include <algorithm>
using namespace std;
#define ONE_ITEMS_MAX_COUNT 5
#define MAX_PAGE_NUMBER		5
const int const_max_items_in_page = 5;
const int const_total_page_btn_count = 5;
const int const_nButtonOffset	  = 20;
const int const_button_gap		  = 20;

bool _greater(const JP_CONSTRACT_INFO& info1,const JP_CONSTRACT_INFO& info2)
{
	return (_tcsicmp(info1.strTime.c_str(),info2.strTime.c_str()) > 0);
}
JPContractSelect::JPContractSelect(CPaintManagerUI* ppm)
:m_pBtnFirstpage(NULL)
,m_pBtnPrevPage(NULL)
,m_pBtnNextPage(NULL)
,m_pBtnEndPage(NULL)
,m_pLabelIdentityTag(NULL)
,m_pListInfo(NULL)
{
	m_PaintManager = ppm;
	dealType.insert(pair<tstring,tstring>(_T("0"),_T("全部")));
	dealType.insert(pair<tstring,tstring>(_T("1"),_T("不委托平台记账")));
	dealType.insert(pair<tstring,tstring>(_T("2"),_T("担保交易")));
	dealType.insert(pair<tstring,tstring>(_T("3"),_T("协议支付(周结)")));

	CDialogBuilder DlgBuilder;
	CVerticalLayoutUI *pProjectPage = NULL;
	CDialogBuilderCallbackEx cb(m_PaintManager);
	if (!DlgBuilder.GetMarkup()->IsValid())
	{
		pProjectPage = static_cast<CVerticalLayoutUI*>(DlgBuilder.Create(XML_CONTRACT_MAIN_PAGE_NAME,
			(UINT)0, &cb, m_PaintManager));
	}
	else
	{
		pProjectPage = static_cast<CVerticalLayoutUI*>(DlgBuilder.Create((UINT)0, m_PaintManager));
	}
	static_cast<CTabLayoutUI*>(ppm->FindControl("SidebarTabContainer"))->Add(pProjectPage);	
	m_PaintManager->AddNotifier(this);

	m_pLabelIdentityTag = static_cast<CLabelUI*>(m_PaintManager->FindControl(LABEL_CONTRACT_IDENTITY_TAG));
	m_pListInfo = static_cast<CListUI*>(m_PaintManager->FindControl(LIST_CONTRACT_ITEMS));

	m_pBtnFirstpage = static_cast<CButtonUI*>(m_PaintManager->FindControl(BTN_LIST_PAGE_FIRST));
	m_pBtnPrevPage = static_cast<CButtonUI*>(m_PaintManager->FindControl(BTN_LIST_PAGE_PREV));
	m_pBtnNextPage = static_cast<CButtonUI*>(m_PaintManager->FindControl(BTN_LIST_PAGE_NEXT));
	m_pBtnEndPage = static_cast<CButtonUI*>(m_PaintManager->FindControl(BTN_LIST_PAGE_END));

	m_pContractFiles = static_cast<CTileLayoutResPacket*>(m_PaintManager->FindControl(LIST_CONTRACT_FILES));
	HideBtn();
}
JPContractSelect::~JPContractSelect(void)
{
	if(NULL != m_pLabelIdentityTag)
	{
		delete m_pLabelIdentityTag;
		m_pLabelIdentityTag = NULL;
	}

	if(NULL != m_pListInfo)
	{
		delete m_pListInfo;
		m_pListInfo = NULL;
	}

	if(NULL != m_pBtnFirstpage)
	{
		delete m_pBtnFirstpage;
		m_pBtnFirstpage = NULL;
	}
	if(NULL != m_pBtnEndPage)
	{
		delete m_pBtnEndPage;
		m_pBtnEndPage = NULL;
	}
	if(NULL != m_pBtnNextPage)
	{
		delete m_pBtnNextPage;
		m_pBtnNextPage = NULL;
	}
	if(NULL != m_pBtnPrevPage)
	{
		delete m_pBtnPrevPage;
		m_pBtnPrevPage = NULL;
	}
}
void JPContractSelect::InitData()
{
	SelectItem(0);
}
//! 收款人/付款人 切换
void JPContractSelect::SelectItem(int nIndex,bool isSearch)
{
	CleanPage();
	CEditUI* pEditKey = static_cast<CEditUI *>(m_PaintManager->FindControl(EDIT_SEARCH_KEY_WORD));
	tstring strKey="";
	if(NULL != pEditKey)
	{
		strKey = pEditKey->GetText();
	}

	CDateTimeUI* pDatesTime = static_cast<CDateTimeUI *>(m_PaintManager->FindControl(DATE_SEARCH_START_TIME));
	tstring strSTime="";
	if(NULL != pDatesTime)
	{
		strSTime = pDatesTime->GetText();
		strSTime += "+00:00:00";
	}

	CDateTimeUI* pDateeTime = static_cast<CDateTimeUI *>(m_PaintManager->FindControl(DATE_SEARCH_END_TIME));
	tstring strETime="";
	if(NULL != pDateeTime)
	{
		strETime = pDateeTime->GetText();
		strETime += "+23:59:59";
	}

	CComboUI* pCombType = static_cast<CComboUI *>(m_PaintManager->FindControl(COMB_SEARCH_PAY_TYPE));
	tstring strType="";
	if(NULL != pCombType)
	{
		int nIndex = pCombType->GetCurSel();
		if(nIndex > 0)
		{
			char temp[8] = {0};
			itoa(nIndex,temp,10);
			strType = temp;
		}
	}
	if(0 == nIndex)
	{
		isPayee = true;
		m_pLabelIdentityTag->SetText(_T("付款方："));
		m_vecPayee.clear();
		if(isSearch)
			GetContractInfo(m_vecPayee,CONTRACT_IDENTITY_PAYEE,strType,strSTime,strETime,strKey);
		else
			GetContractInfo(m_vecPayee,CONTRACT_IDENTITY_PAYEE);
		m_nEndPageIndex = m_vecPayee.size();
		if(m_nEndPageIndex > 0)
		{
			JP_CONSTRACT_INFO& info = m_vecPayee[0];
			ShowItemInfo(&info);
		}
	}
	else
	{
		isPayee = false;
		m_pLabelIdentityTag->SetText(_T("收款方："));
		m_vecDrawee.clear();
		if(isSearch)
			GetContractInfo(m_vecDrawee,CONTRACT_IDENTITY_DRAWEE,strType,strSTime,strETime,strKey);
		else
			GetContractInfo(m_vecDrawee,CONTRACT_IDENTITY_DRAWEE);
		m_nEndPageIndex = m_vecDrawee.size();
		if(m_nEndPageIndex > 0)
		{
			JP_CONSTRACT_INFO& info = m_vecDrawee[0];
			ShowItemInfo(&info);
		}
	}
	m_pListInfo->RemoveAll();

	int size = 0;
	if(isPayee)
	{
		size = m_vecPayee.size();
	}
	else
	{
		size = m_vecDrawee.size();
	}
	UpdateParas(size);
	UpdatePageButton();
	if(size > ONE_ITEMS_MAX_COUNT)
	{
		AddListItem(0,ONE_ITEMS_MAX_COUNT);
	}
	else
	{
		AddListItem(0,size);
	}
}
void JPContractSelect::UpdateParas(int nCount)
{
	int nPageCount = nCount/ONE_ITEMS_MAX_COUNT;
	int nLeftItemCount = nCount%ONE_ITEMS_MAX_COUNT;
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

void JPContractSelect::HideBtn()
{
	for (int i=1; i<=MAX_PAGE_NUMBER; i++)
	{
		char buf[MAX_PATH]={0};
		sprintf_s(buf, sizeof(buf), _T("_btn_page_%d"), i);
		CControlUI* pControlPagebtn= (CControlUI*)static_cast<CControlUI*>(m_PaintManager->FindControl(buf));
		if (NULL != pControlPagebtn)
		{
			sprintf_s(buf, sizeof(buf), _T("%d"), i);
			pControlPagebtn->SetAttribute("textcolor", "#ff000000");
			pControlPagebtn->SetText(buf);
			pControlPagebtn->SetVisible(false);
		}
	}
}
//如果页面超过5页。显示的区间为：m_nCurrentPageindex-2, m_nCurrentPageIndex+2
void JPContractSelect::UpdatePageButton()
{
	if (m_nEndPageIndex>1)
	{
		HideBtn();
		//1：先显示按钮：
		int nButtonCount = 0;
		//1-5个
		if (m_nEndPageIndex<=const_total_page_btn_count)
		{
			nButtonCount = m_nEndPageIndex;
		}
		//>5个
		else
		{
			nButtonCount = const_total_page_btn_count;
		}

		//先显示4个按钮。
		m_pBtnFirstpage->SetVisible(true);
		m_pBtnPrevPage->SetVisible(true);
		m_pBtnNextPage->SetVisible(true);
		m_pBtnEndPage->SetVisible(true);

		//初始化1--5(最多)个
		for (int i=1; i<=nButtonCount; i++)
		{
			char buf[MAX_PATH]={0};
			sprintf_s(buf, sizeof(buf), _T("_btn_page_%d"), i);
			CControlUI* pControlPagebtn= (CControlUI*)static_cast<CControlUI*>(m_PaintManager->FindControl(buf));
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
		//只有多于5页的才有可能刷新按钮
		if (m_nEndPageIndex > const_total_page_btn_count)
		{
			if (m_nCurrentPageIndex-2<=0)
			{
				nBeginIndex = 1;
				nEndindex   = const_total_page_btn_count;
			}
			else if (m_nCurrentPageIndex+2>=m_nEndPageIndex)
			{
				nBeginIndex = m_nEndPageIndex-4;
				nEndindex   = m_nEndPageIndex;
				//bNeedUpdate = true;
			}
			else
			{
				nBeginIndex = m_nCurrentPageIndex-2;
				nEndindex = m_nCurrentPageIndex+2;
				//bNeedUpdate = true;
			}

			//if (bNeedUpdate)
			{
				//更新按钮数值
				for (int i=1; i<=const_total_page_btn_count; i++)
				{
					char buf[MAX_PATH]={0};
					sprintf_s(buf, sizeof(buf), _T("_btn_page_%d"), i);
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

		//3:并再高亮当前页按钮！
		for (int i=1; i<=nButtonCount; i++)
		{
			char buf[MAX_PATH]={0};
			sprintf_s(buf, sizeof(buf), _T("_btn_page_%d"), i);
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


		//刷新button位置
		if (NULL != m_pBtnPrevPage)
		{
			//下一页
			SIZE szPos = m_pBtnPrevPage->GetFixedXY();
			int nBtnWidth = m_pBtnPrevPage->GetFixedWidth();
			szPos.cx = szPos .cx + nBtnWidth + nButtonCount*const_nButtonOffset + const_button_gap;
			m_pBtnEndPage->SetFixedXY(szPos);

			//尾页
			szPos.cx += const_button_gap;
			m_pBtnNextPage->SetFixedXY(szPos);
		}

	}	
	//没有页数，就不显示四个按钮
	else
	{
		if (NULL != m_pBtnPrevPage)
		{
			m_pBtnFirstpage->SetVisible(false);
			m_pBtnPrevPage->SetVisible(false);
			m_pBtnNextPage->SetVisible(false);
			m_pBtnEndPage->SetVisible(false);
			HideBtn();
		}
	}
}
void JPContractSelect::ShowItemInfo(JP_CONSTRACT_INFO* info)
{
	CLabelUI* pLabelID = static_cast<CLabelUI *>(m_PaintManager->FindControl(LABEL_CONTRACT_ID));
	if(NULL != pLabelID)
	{
		pLabelID->SetText(info->strNO.c_str());
	}

	CLabelUI* pLabelName = static_cast<CLabelUI *>(m_PaintManager->FindControl(LABEL_CONTRACT_NAME));
	if(NULL != pLabelName)
	{
		pLabelName->SetText(info->strName.c_str());
	}

	CLabelUI* pLabelIdent = static_cast<CLabelUI *>(m_PaintManager->FindControl(LABEL_CONTRACT_IDENTITY));
	if(NULL != pLabelIdent)
	{
		pLabelIdent->SetText(info->strIdentity.c_str());
	}

	CLabelUI* pLabelType = static_cast<CLabelUI *>(m_PaintManager->FindControl(LABEL_CONTRACT_PAY_TYPR));
	if(NULL != pLabelType)
	{
		pLabelType->SetText(dealType[info->strType].c_str());
	}

	CLabelUI* pLabelMoney = static_cast<CLabelUI *>(m_PaintManager->FindControl(LABEL_CONTRACT_MONEY));
	if(NULL != pLabelMoney)
	{
		pLabelMoney->SetText(info->strMoney.c_str());
	}

	CLabelUI* pLabelTime = static_cast<CLabelUI *>(m_PaintManager->FindControl(LABEL_CONTRACT_TIME));
	if(NULL != pLabelTime)
	{
		pLabelTime->SetText(info->strTime.c_str());
	}

	CLabelUI* pLabelRemark = static_cast<CLabelUI *>(m_PaintManager->FindControl(LABEL_CONTRACT_REMARK));
	if(NULL != pLabelRemark)
	{
		pLabelRemark->SetText(info->strMark.c_str());
	}

	m_pContractFiles->RemoveAll();
	for(int i = 0;i<info->m_vecFiles.size();i++)
	{
		m_pContractFiles->AddOneItem(&(info->m_vecFiles[i]),false);
	}
}
void JPContractSelect::AddListItem(int nFromIndex, int nCount)
{
	for (int i=nFromIndex; i<nCount+nFromIndex; i++)
	{
		CDialogBuilder DlgBuilder;
		CListContainerElementUI *plstItem = NULL;
		if (!DlgBuilder.GetMarkup()->IsValid())
		{
			plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create(XML_ITEM_CONTRACT_NAME,
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
			JP_CONSTRACT_INFO oneInfo;
			if(isPayee)
				oneInfo = m_vecPayee[i];
			else
				oneInfo = m_vecDrawee[i];
			JP_CONSTRACT_INFO* pNewData = new JP_CONSTRACT_INFO;
			pNewData->strIdentity = oneInfo.strIdentity;
			pNewData->strType = oneInfo.strType;
			pNewData->strMark = oneInfo.strMark;
			pNewData->strMoney = oneInfo.strMoney;
			pNewData->strName = oneInfo.strName;
			pNewData->strNO = oneInfo.strNO;
			pNewData->strTime = oneInfo.strTime;
			pNewData->strOrgid = oneInfo.strOrgid;

			for(int i = 0;i<oneInfo.m_vecFiles.size();i++)
			{
				fileInfo file;
				file.strFileName = oneInfo.m_vecFiles[i].strFileName;
				file.strFileType = oneInfo.m_vecFiles[i].strFileType;
				file.strFileSz = oneInfo.m_vecFiles[i].strFileSz;
				file.dwFileSize = oneInfo.m_vecFiles[i].dwFileSize;
				file.strFileUrl = oneInfo.m_vecFiles[i].strFileUrl;
				pNewData->m_vecFiles.push_back(file);
			}
			pNewData->m_vecFiles = oneInfo.m_vecFiles;

			char strTmp[MAX_PATH] = {0};
			CLabelUI* pLabelWho = static_cast<CLabelUI *>(m_PaintManager->FindSubControlByName(plstItem, LABEL_ITEM_CONTRACT_WHO));
			if(pLabelWho != NULL)
			{
				pLabelWho->SetText(oneInfo.strIdentity.c_str());
			}

			CLabelUI* pLabelName = static_cast<CLabelUI *>(m_PaintManager->FindSubControlByName(plstItem, LABEL_ITEM_CONTRACT_NAME));
			if(pLabelName != NULL)
			{
				pLabelName->SetText(oneInfo.strName.c_str());
			}

			CLabelUI* pLabelMoney = static_cast<CLabelUI *>(m_PaintManager->FindSubControlByName(plstItem, LABEL_ITEM_CONTRACT_MONEY));
			if(pLabelMoney != NULL)
			{
				pLabelMoney->SetText(oneInfo.strMoney.c_str());
			}

			CLabelUI* pLabelTime = static_cast<CLabelUI *>(m_PaintManager->FindSubControlByName(plstItem, LABEL_ITEM_CONTRACT_TIME));
			if(pLabelTime != NULL)
			{
				pLabelTime->SetText(oneInfo.strTime.c_str());
			}

			plstItem->SetTag((UINT_PTR)pNewData);
			if (!m_pListInfo->Add(plstItem))
			{
				delete plstItem;
			}
		}
	}
}
void JPContractSelect::CleanPage()
{
	CLabelUI* pLabelID = static_cast<CLabelUI *>(m_PaintManager->FindControl(LABEL_CONTRACT_ID));
	if(NULL != pLabelID)
	{
		pLabelID->SetText("");
	}

	CLabelUI* pLabelName = static_cast<CLabelUI *>(m_PaintManager->FindControl(LABEL_CONTRACT_NAME));
	if(NULL != pLabelName)
	{
		pLabelName->SetText("");
	}

	CLabelUI* pLabelIdent = static_cast<CLabelUI *>(m_PaintManager->FindControl(LABEL_CONTRACT_IDENTITY));
	if(NULL != pLabelIdent)
	{
		pLabelIdent->SetText("");
	}

	CLabelUI* pLabelType = static_cast<CLabelUI *>(m_PaintManager->FindControl(LABEL_CONTRACT_PAY_TYPR));
	if(NULL != pLabelType)
	{
		pLabelType->SetText("");
	}

	CLabelUI* pLabelMoney = static_cast<CLabelUI *>(m_PaintManager->FindControl(LABEL_CONTRACT_MONEY));
	if(NULL != pLabelMoney)
	{
		pLabelMoney->SetText("");
	}

	CLabelUI* pLabelTime = static_cast<CLabelUI *>(m_PaintManager->FindControl(LABEL_CONTRACT_TIME));
	if(NULL != pLabelTime)
	{
		pLabelTime->SetText("");
	}

	CLabelUI* pLabelRemark = static_cast<CLabelUI *>(m_PaintManager->FindControl(LABEL_CONTRACT_REMARK));
	if(NULL != pLabelRemark)
	{
		pLabelRemark->SetText("");
	}
}
void JPContractSelect::GotoPage(int nPageIndex)
{
	if (nPageIndex>0 && nPageIndex<= m_nEndPageIndex)
	{
		//先清空当前的list
		RemoveAllListItems();

		int nStartIndexInCache = (nPageIndex-1)*const_max_items_in_page;
		int nShowCount = const_max_items_in_page;
		if (nPageIndex == m_nEndPageIndex)//如果是最后一页:减去前n-1页就是剩下的的：1-8个
		{
			if(isPayee)
				nShowCount = m_vecPayee.size() - (nPageIndex-1)*const_max_items_in_page;
			else
				nShowCount = m_vecDrawee.size() - (nPageIndex-1)*const_max_items_in_page;
		}

		AddListItem(nStartIndexInCache, nShowCount);

		m_nCurrentPageIndex = nPageIndex;

		//将当前页的字体设为高亮
		UpdatePageButton();
	}
}
bool JPContractSelect::IsCacheEmpty()
{
	int size = 0;
	if(isPayee)
		size = m_vecPayee.size();
	else
		size = m_vecDrawee.size();
	if (size == 0)
	{
		CMsgBox* pMsgBox = new CMsgBox("失败","没有可以显示的项目！", MSGBOX_ICON_ERROR, MSGBOX_IDOK);
		pMsgBox->ShowModal();

		return true;
	}

	return false;
}
void JPContractSelect::RemoveAllListItems()
{
	int nCount = m_pListInfo->GetCount();
	if (nCount>0)
	{
		for (int i=0; i<nCount; i++)
		{
			CListContainerElementUI* pListItem = static_cast<CListContainerElementUI*>(m_pListInfo->GetItemAt(i));
			if (NULL != pListItem)
			{
				JP_CONSTRACT_INFO* pProjData = (JP_CONSTRACT_INFO*)pListItem->GetTag();
				if (NULL != pProjData)
				{
					delete pProjData;
					pProjData = NULL;
				}
			}
		}

		m_pListInfo->RemoveAll();
	}

	//还原Index
	m_nCurrentPageIndex = -1;
}
bool JPContractSelect::IsFirstPage()
{
	if (m_nCurrentPageIndex == 1)
	{
		CMsgBox* pMsgBox = new CMsgBox("提醒","当前已经是第一页了！", MSGBOX_ICON_WARNING, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return true;
	}

	return false;
}
bool JPContractSelect::GetContractInfo(vector<JP_CONSTRACT_INFO>& vecInfo,const tstring status /* = "1" */,const tstring type /* = "" */,const tstring& stime/* ="" */,const tstring& etime/* ="" */,const tstring& key/* ="" */)
{
	tstring strRespons;

	CWebClient::getContractInfo(strRespons,status,type,stime,etime,key);
	if(!strRespons.empty() || _tcsicmp(strRespons.c_str(),"[]") == 0)
	{
		Json::Reader reader;
		Json::Value root;

		try
		{
			if(reader.parse(strRespons,root))
			{
				int nSize = root.size();
				for (int i=0; i<nSize; i++)
				{
					JP_CONSTRACT_INFO info;
					info.strIdentity = root[i]["org_name"].asString();
					info.strOrgid = root[i]["oid"].asString();
					info.strMoney = root[i]["money"].asString();
					info.strName = root[i]["pname"].asString();
					info.strNO = root[i]["id"].asString();
					info.strTime = root[i]["create_time"].asString();
					int iType = root[i]["pay_type"].asInt();
					char temp[4] = {0};
					itoa(iType,temp,10);
					info.strType = temp;
					info.strMark = root[i]["desc"].asString();

					tstring strTemp;
					CWebClient::getContractFiles(strTemp,info.strNO,info.strOrgid);
					Json::Reader readerFile;
					Json::Value rootFile;
					if(readerFile.parse(strTemp,rootFile) && -1 != strTemp.find(_T("\"file_list\":[{")))
					{
						int nSize = rootFile.size();
						if(nSize > 0)
						{
							int iFilesize = rootFile["file_list"].size();
							for (int j=0; j<iFilesize; j++)
							{
								fileInfo file;
								file.strFileName = rootFile["file_list"][j]["file_name"].asString();
								file.strFileType = rootFile["file_list"][j]["file_type"].asString();
								file.strFileSz = rootFile["file_list"][j]["file_size"].asString();
								file.dwFileSize = atol(file.strFileSz.c_str());
								file.strFileUrl = rootFile["file_list"][j]["ali_file_url"].asString();
								info.m_vecFiles.push_back(file);
							}
						}
					}
					vecInfo.push_back(info);
				}
				if(vecInfo.size() > 0)
					sort(vecInfo.begin(),vecInfo.end(),_greater);
			}
			else
				return false;

		}catch(...)
		{
			CLogger::GetInstance()->PrintErrLog("获取合约信息,解析数据失败");
			return false;
		}
	}
	else
		return false;

	return true;
}
bool JPContractSelect::IsEndPage()
{
	if (m_nCurrentPageIndex == m_nEndPageIndex)
	{
		CMsgBox* pMsgBox = new CMsgBox("提醒","当前页已经是最后一页了！", MSGBOX_ICON_WARNING, MSGBOX_IDOK);
		pMsgBox->ShowModal();
		return true;
	}

	return false;
}
void JPContractSelect::OnSearchButtonClick()
{
	if(isPayee)
	{
		SelectItem(0,true);
	}
	else
	{
		SelectItem(1,true);
	}
}

void JPContractSelect::Notify(TNotifyUI& msg)
{
	if(_tcsicmp(msg.sType,"click") == 0)
	{
		DuiLib::CDuiString strName = msg.pSender->GetName();
		if(_tcsicmp(strName.GetData(),RADIO_CONTRACT_PAYEE) == 0)
		{
			SelectItem(0);
		}
		else if(_tcsicmp(strName.GetData(),RADIO_CONTRACT_DRAWEE) == 0)
		{
			SelectItem(1);
		}
		else if(_tcsicmp(strName.GetData(),BTN_LIST_PAGE_NEXT) == 0)
		{
			if (!IsCacheEmpty() && !IsEndPage())
			{
				if (m_nCurrentPageIndex<m_nEndPageIndex)
				{
					GotoPage(m_nCurrentPageIndex+1);
				}
			}
		}
		else if(_tcsicmp(strName.GetData(),BTN_LIST_PAGE_PREV) == 0)
		{
			if (!IsCacheEmpty() && !IsFirstPage())
			{
				if (m_nCurrentPageIndex>1)
				{
					GotoPage(m_nCurrentPageIndex-1);
				}
			}
		}
		else if(_tcsicmp(strName.GetData(),BTN_LIST_PAGE_FIRST) == 0)
		{
			if (!IsCacheEmpty()&& !IsFirstPage())
			{
				GotoPage(1);
			}
		}
		else if(_tcsicmp(strName.GetData(),BTN_LIST_PAGE_END) == 0)
		{
			if (!IsCacheEmpty()&& !IsEndPage())
			{
				if (m_nEndPageIndex>0)
				{
					GotoPage(m_nEndPageIndex);
				}	
			}
		}
		else if (strName.Find("_btn_page_")>=0)
		{
			int nIndex = atoi(msg.pSender->GetText());
			GotoPage(nIndex);
			return;
		}
		else if(_tcsicmp(strName.GetData(),BTN_SEARCH_OK) == 0)
		{
			OnSearchButtonClick();
		}
	}
	else if(_tcsicmp(msg.sType,"itemclick") == 0)
	{
		if(_tcsicmp(msg.pSender->GetName(),ELEMENT_ITEM_NAME) == 0)
		{
			if (_tcsicmp(msg.pSender->GetClass(), _T("ListContainerElementUI")) == 0)
			{
				JP_CONSTRACT_INFO* info = (JP_CONSTRACT_INFO*)msg.pSender->GetTag();
				if(NULL != info)
					ShowItemInfo(info);
			}
		}
	}
}