#include "FrameEmployee.h"
#include "mainDlg.h"
#include "Controller.h"
#include "UserManager.h"
#include "../json/json.h"
#include "../Common/regexHelper.h"
#include "MsgBox.h"
#include "StringHelper.h"

CFrameEmployee* CFrameEmployee::m_pInstance = NULL;
CFrameEmployee::CFrameEmployee()
{
	CController::Instance()->GetUserInfoMgr()->AddEventHandler(this);
}


CFrameEmployee::~CFrameEmployee(void)
{

}

CFrameEmployee* CFrameEmployee::Instance()
{
	if(NULL == m_pInstance)
	{
		m_pInstance = new CFrameEmployee;
		m_pInstance->CreateWnd(mainDlg::getInstance()->GetHWND());
	}
	return m_pInstance;
}

void CFrameEmployee::HandleSetFocusEvent( TNotifyUI& msg )
{

}

void CFrameEmployee::HandleKillFocusEvent( TNotifyUI& msg )
{

}

void CFrameEmployee::HandleTextChangedEvent( TNotifyUI& msg )
{

}

void CFrameEmployee::HandleClickEvent( TNotifyUI& msg )
{
	if (_tcsicmp(msg.pSender->GetName(), _T("buttonAdd")) == 0)
	{
		tstring strPhone = m_pEditPhone->GetText();
		AddMember(strPhone);
	}
	else if (_tcsicmp(msg.pSender->GetName(), _T("buttonClose")) == 0)
	{
		/*CMsgBox* pMsgBox = new CMsgBox("老板权限转让", "是否确定转让权限？",MSGBOX_ICON_ASK, MSGBOX_IDYESNO);
		if (MSGBOX_IDNO == pMsgBox->ShowModal(NULL))
		{			
			return;
		}		*/
		ShowWindow(false);
		InitWindow();
	}
}

void CFrameEmployee::HandleSelectedChangedEvent( TNotifyUI& msg )
{

}

void CFrameEmployee::HandleItemSelectEvent( TNotifyUI& msg )
{

}

DuiLib::CDuiString CFrameEmployee::GetSkinFile()
{
	return _T("sys_employee.xml");
}

CControlUI* CFrameEmployee::CreateControl( LPCTSTR pstrClass )
{
	return NULL;
}

void CFrameEmployee::InitWindow()
{
	m_plstEmployee = static_cast<CListUI*>(m_PaintManager.FindControl(_T("lstEmployee")));
	m_pEditPhone = static_cast<CEditUI *>(m_PaintManager.FindControl(_T("editPhone")));
	m_plstEmployee->RemoveAll();
	if (m_plstEmployee == NULL || m_pEditPhone == NULL)
		return;
	if (!getOrgId())
		return;
	m_vctMembers.clear();
	getOrgMembers(m_strOid, m_vctMembers);
	if (m_vctMembers.size() > 0)
	{
		for (int i=0; i<m_vctMembers.size(); i++)
		{
			//3. 获取个人vcard
			CController::Instance()->GetUserInfoMgr()->FetchVCardByAccount(m_vctMembers[i].strPhone);
			//4. 加到ui
			AddItem(m_vctMembers[i]);
		}
	}
}
bool CFrameEmployee::updateItem( OrgMemberInfo memInfo )
{
	CListContainerElementUI *plstItem = static_cast<CListContainerElementUI*>(m_PaintManager.FindControl(memInfo.strPhone.c_str()));
	if (plstItem == NULL)
		return false;
	else
	{
		CLabelUI *plablNick = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("labelNick")));
		if (plablNick != NULL)
		{
			plablNick->SetText(memInfo.strNickName.c_str());
		}

		CControlUI *pControlLogo = static_cast<CControlUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("controlLogo")));
		if (pControlLogo != NULL)
		{
			bool bExist = PathFileExists(memInfo.strLogo.c_str());
			if (!bExist || memInfo.strLogo.empty())
			{
				pControlLogo->SetBkImage(DEF_AVATAR_NOR);
			}
			else
			{
				pControlLogo->SetBkImage(memInfo.strLogo.c_str());
			}
		}
		return true;
	}
}
void CFrameEmployee::OnVCardReturned( tstring& strAccount, sUserVCard& oneInfo )
{
	for (int i=0; i<m_vctMembers.size(); i++)
	{
		if ((m_vctMembers[i].strPhone) == oneInfo.strPhone)
		{
			m_vctMembers[i].strLogo = oneInfo.strPhotoPath;
			m_vctMembers[i].strNickName = oneInfo.strUserNickname;
			m_vctMembers[i].strAccount = oneInfo.strAccount;
			updateItem(m_vctMembers[i]);
			return;
		}
	}
}

void CFrameEmployee::OnMySelfCardInfo( sUserVCard& oneInfo )
{
	for (int i=0; i<m_vctMembers.size(); i++)
	{
		if ((m_vctMembers[i].strPhone) == oneInfo.strPhone)
		{
			m_vctMembers[i].strLogo = oneInfo.strPhotoPath;
			m_vctMembers[i].strNickName = oneInfo.strUserNickname;
			m_vctMembers[i].strAccount = oneInfo.strAccount;
			updateItem(m_vctMembers[i]);
			return;
		}
	}
}

bool CFrameEmployee::getOrgId()
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-org");
	tstring strPost = (_T(""));
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Post(strUrl, strPost, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1 || strHttpResponse.find("\"id\":\"") == -1)
	{
		return false;
	}
	else
	{
		Json::Reader reader;
		Json::Value root;
		try
		{
			if (reader.parse(strHttpResponse, root))
			{
				m_strOid = root["id"].asString();
			}
		}
		catch (...)
		{
			return false;
		}	
		return true;
	}
}

bool CFrameEmployee::getOrgMembers( tstring strOrgId, vector<OrgMemberInfo>& vctMembers )
{
	//m_strMySelfPhone = mainDlg::getInstance()->getMyUserInfo().strPhone;
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-org-members?");
	tstring strPost = _T("oid=") + m_strOid;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);

	if (!strHttpResponse.empty() && strHttpResponse.find("members") != -1)
	{
		Json::Reader reader;
		Json::Value root;
		if (reader.parse(strHttpResponse, root))
		{
			int nSize = root["members"].size();
			for (int i=0; i<nSize; i++)
			{


				OrgMemberInfo oneInfo;
				oneInfo.strPhone = root["members"][i]["cell_phone"].asString();
				tstring strDuty = root["members"][i]["duty"].asString();
				if (strDuty.find("null") != -1)
				{
					oneInfo.brequirement = false;
					oneInfo.bengage = false;
					oneInfo.bexecute = false;
					oneInfo.baftersale = false;
				}
				else
				{
					int nIndex = strDuty.find("1");
					if (nIndex >= 0)
						oneInfo.brequirement = true;
					else
						oneInfo.brequirement = false;

					nIndex = strDuty.find("2");
					if (nIndex >= 0)
						oneInfo.bengage = true;
					else
						oneInfo.bengage = false;

					nIndex = strDuty.find("3");
					if (nIndex >= 0)
						oneInfo.bexecute = true;
					else
						oneInfo.bexecute = false;

					nIndex = strDuty.find("4");
					if (nIndex >= 0)
						oneInfo.baftersale = true;
					else
						oneInfo.baftersale = false;
				}

				if (root["members"][i]["is_default"].asInt() == 1)
					m_strRecipient = oneInfo.strPhone;
				m_vctMembers.push_back(oneInfo);
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool CFrameEmployee::getRecipient()
{
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/get-default?");
	tstring strPost = (_T(""));
	strPost += _T("oid=");
	strPost += m_strOid;
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(strUrl, strToken, strHttpResponse);
	if (strHttpResponse.find("\"result\":0") != -1 || strHttpResponse.find("cell_phone") == -1)
	{
		return false;
	}
	else
	{
		Json::Reader reader;
		Json::Value root;
		try
		{
			if (reader.parse(strHttpResponse, root))
			{
				m_strRecipient = root["cell_phone"].asString();
			}
		}
		catch (...)
		{
			return false;
		}	



		//todo
		//m_strRecipient = strHttpResponse.substr(strHttpResponse.find("\"cell_phone\":\"") + 14, strHttpResponse.find("\",\"") - (strHttpResponse.find("\"cell_phone\":\"") + 14));
		return true;
	}
}

bool CFrameEmployee::AddItem( OrgMemberInfo memInfo )
{
	CDialogBuilder DlgBuilder;
	CListContainerElementUI *plstItem = NULL;
	if (!DlgBuilder.GetMarkup()->IsValid())
	{
		plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create(_T("sys_employee_item.xml"),
			(UINT)0, NULL, &m_PaintManager));
	}
	else
	{
		plstItem = static_cast<CListContainerElementUI*>(DlgBuilder.Create((UINT)0, &m_PaintManager));
	}
	if (plstItem == NULL)
		return false;
	else
	{
		CLabelUI *plabelPhone = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("labelcellphone")));
		if (plabelPhone!= NULL)
			plabelPhone->SetText(memInfo.strPhone.c_str());
		plstItem->SetName(memInfo.strPhone.c_str());
		CLabelUI *plablNick = static_cast<CLabelUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("labelNick")));
		if (plablNick != NULL)
		{
			if (memInfo.strNickName == "")
			{
				plablNick->SetText(memInfo.strPhone.c_str());
			}
			else
				plablNick->SetText(memInfo.strNickName.c_str());
		}

		CControlUI *pControlLogo = static_cast<CControlUI *>(m_PaintManager.FindSubControlByName(plstItem, _T("controlLogo")));
		if (pControlLogo != NULL)
		{
			bool bExist = PathFileExists(memInfo.strLogo.c_str());
			if (!bExist || memInfo.strLogo.empty())
			{
				pControlLogo->SetBkImage(DEF_AVATAR_NOR);
			}
			else
			{
				pControlLogo->SetBkImage(memInfo.strLogo.c_str());
			}
		}

		COptionUI *pOptionRecipient = static_cast<COptionUI*> (m_PaintManager.FindSubControlByName(plstItem, _T("option_recipient")));
		if (pOptionRecipient != NULL)
		{
			if (m_strRecipient == memInfo.strPhone)
				pOptionRecipient->Selected(true);
			else
				pOptionRecipient->Selected(false);
		}

		COptionUI *pOptionrequirement = static_cast<COptionUI*> (m_PaintManager.FindSubControlByName(plstItem, _T("Option_requirement")));
		if (pOptionrequirement != NULL)
		{
			if (memInfo.brequirement)
				pOptionrequirement->Selected(true);
			else
				pOptionrequirement->Selected(false);
		}
		COptionUI *pOptionEngage = static_cast<COptionUI*> (m_PaintManager.FindSubControlByName(plstItem, _T("Option_engage")));
		if (pOptionEngage != NULL)
		{
			if (memInfo.bengage)
				pOptionEngage->Selected(true);
			else
				pOptionEngage->Selected(false);
		}
		COptionUI *pOptionexecute = static_cast<COptionUI*> (m_PaintManager.FindSubControlByName(plstItem, _T("Option_execute")));
		if (pOptionexecute != NULL)
		{
			if (memInfo.bexecute)
				pOptionexecute->Selected(true);
			else
				pOptionexecute->Selected(false);
		}
		COptionUI *pOptionaftersale = static_cast<COptionUI*> (m_PaintManager.FindSubControlByName(plstItem, _T("Option_aftersale")));
		if (pOptionaftersale != NULL)
		{
			if (memInfo.baftersale) 
				pOptionaftersale->Selected(true);
			else
				pOptionaftersale->Selected(false);
		}

		plstItem->SetFixedHeight(40);
		if (!m_plstEmployee->Add(plstItem))
		{
			delete plstItem;
			return false;
		}
		return true;
	}
}

bool CFrameEmployee::AddMember( tstring strPhone )
{
	if (!CRegexHelper::IsValidMobileNo(strPhone.c_str()))
	{
		CMsgBox* pMsgBox = new CMsgBox("查找", "输入的手机号码格式错误",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return false;
	}
	for (int i=0; i<m_vctMembers.size(); i++)
	{
		if (m_vctMembers[i].strPhone == strPhone)
		{
			CMsgBox* pMsgBox = new CMsgBox("查找", "账号已添加，请不要重复添加",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
			return false;
		}
	}
	OrgMemberInfo info;
	info.strAccount = strPhone + "@test-d";
	info.strPhone = strPhone;
	info.m_bVerify = true;

	//调用web端接口
	CHttpClient httpClient;
	tstring strUrl(WEB_SERVER_BASE_URL);
	strUrl += _T("org/add-members?");
	tstring strPost = (_T(""));
	strPost += _T("oid=");
	tstring strNameDefault = m_strOid;
	strPost += strNameDefault;
	strPost += _T("&cell_phones=[") + strPhone + "]";           //110000,110100,110101");
	strUrl += strPost;
	tstring strToken = CController::Instance()->GetToken();
	tstring strHttpResponse;
	int retCode = httpClient.Get(CChineseCode::EncodeUTF8(strUrl), strToken, strHttpResponse);
	if (strHttpResponse.find("success_phones") == -1 || strHttpResponse.length() < 31)
	{
		if (strHttpResponse.find("exist_phones") != -1)
		{
			CMsgBox* pMsgBox = new CMsgBox("添加", "此账号已有机构, 添加员工失败！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
		}
		else if (strHttpResponse == "{\"success_phones\":[\"\"]}")
		{
			CMsgBox* pMsgBox = new CMsgBox("添加", "此账号非平台账号, 添加员工失败！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
		}
		else
		{
			CMsgBox* pMsgBox = new CMsgBox("添加", "添加员工失败！",  MSGBOX_ICON_WARNING);
			pMsgBox->ShowModal(this->GetHWND());
		}

		return false;
	}
	else
	{
		//成功
		OrgMemberInfo oneinfo;
		oneinfo.baftersale = false;
		oneinfo.brequirement = false;
		oneinfo.bengage = false;
		oneinfo.bexecute = false;
		oneinfo.strPhone = strPhone;
		m_vctMembers.push_back(oneinfo);
		AddItem(oneinfo);
		CController::Instance()->GetUserInfoMgr()->FetchVCardByAccount(strPhone);
		CMsgBox* pMsgBox = new CMsgBox("添加", "添加员工成功！",  MSGBOX_ICON_WARNING);
		pMsgBox->ShowModal(this->GetHWND());
		return true;
	}	
}
